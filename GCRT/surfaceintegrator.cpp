#include "surfaceintegrator.h"

extern _declspec(thread) PathDebugData tls_pathDbgData;

/**
 * InvalidColor Check if a color has any Nan values.
 * @param  color Input color to check.
 * @return       True if color is invalid.
 */

bool InvalidColor(dvec3 color)
{
    if (isnan(color.x) ||
        isnan(color.y) ||
        isnan(color.z)
        )
    {
        return true;
    }

    return false;
}

/**
 * SurfaceIntegrator::DumpPath For debugging. Print a ray path and the intersections
 * it has hit along the way.
 */

void SurfaceIntegrator::DumpPath()
{
    printf("Ray Path Data\n");

    for (uint32_t i = 0; i < tls_pathDbgData.stackPtr; i++)
    {
        printf(
            "Ray %d: Origin = (%2.2f, %2.2f, %2.2f), Origin Mat = %s\n", 
            i,
            tls_pathDbgData.rayOrigins[i].x,
            tls_pathDbgData.rayOrigins[i].y,
            tls_pathDbgData.rayOrigins[i].z,
            RTMaterial::GetName((Material)tls_pathDbgData.intscMaterials[i]).c_str()
        );
    }
}

/**
 * SurfaceIntegrator::SampleSurface For a given incoming ray from a surface,
 * generate sets of sample rays based on material BSDR and light sources and use
 * these samples to approximate the rendering equation.
 *
 * @param  ray        Incoming ray.
 * @param  intsc      Surface intersection point.
 * @param  bounce     Current bounce depth from camera.
 * @param  maxBounces Maximum number of bounces for this camera path.
 * @return            Output rendering equation color along this ray.
 */

dvec3 SurfaceIntegrator::SampleSurface(
    Ray rayIn,
    RTScene &scn,
    Intersection intsc,
    uint32_t bounce,
    uint32_t maxBounces
)
{
    vector<SurfSample> surfSamples(256);
    auto &mat = *scn.mats[intsc.mat];

    uint32_t nBSDFSamples   = 0;
    uint32_t nLightSamples  = 0;
    uint32_t totalSamples   = 0;

    tls_pathDbgData.rayOrigins[tls_pathDbgData.stackPtr] = rayIn.org;
    tls_pathDbgData.intscMaterials[tls_pathDbgData.stackPtr++] = intsc.mat;

    if (bounce < maxBounces)
    {
        nBSDFSamples = SampleBSDF(
            surfSamples,
            bounce,
            maxBounces,
            rayIn,
            scn,
            intsc,
            totalSamples
        );
    }

    nLightSamples = SampleLightDistribution(
        surfSamples,
        bounce,
        maxBounces,
        rayIn,
        scn,
        intsc,
        totalSamples
    );

    dvec3 out = ApplyBalanceHeuristic(surfSamples, nBSDFSamples, nLightSamples);

    if (InvalidColor(out))
    {
        DumpPath();
        __debugbreak();
    }

    tls_pathDbgData.stackPtr--;

    return out;
}

/**
 * SurfaceIntegrator::ApplyBalanceHeuristic Weight together different sets of
 * rendering equation samples obtained from different distributions (e.g., BSDF and
 * light distributions).
 *
 * @param  surfSamples   A list of samples.
 * @param  nBSDFSamples  Number of samples from BSDF.
 * @param  nLightSamples Number of samples from lights.
 *
 * @return               Color obtained by weighting samples from different distributions.
 */

dvec3 SurfaceIntegrator::ApplyBalanceHeuristic(
    vector<SurfSample> &surfSamples,
    uint32_t nBSDFSamples,
    uint32_t nLightSamples
)
{
    dvec3 bsdfTerm = dvec3(0.0);
    dvec3 lightTerm = dvec3(0.0);

    for (uint32_t i = 0; i < (nBSDFSamples + nLightSamples); i++)
    {
        SurfSample &sample = surfSamples[i];

        switch (sample.distType)
        {
        case BSDF_TYPE:

            if (sample.BSDFPDF == 0.0)
            {
                bsdfTerm += sample.BSDF;
            }
            else
            {
                double w = (double)nBSDFSamples * sample.BSDFPDF /
                    ((double)nBSDFSamples * sample.BSDFPDF + (double)nLightSamples * sample.LightPDF);

                bsdfTerm += sample.BSDF * w / sample.BSDFPDF;
            }
            break;

        case LIGHT_TYPE:

            if (sample.BSDFPDF == 0.0)
            {
                continue;
            }
            else
            {
                double w = (double)nLightSamples * sample.LightPDF /
                    ((double)nBSDFSamples * sample.BSDFPDF + (double)nLightSamples * sample.LightPDF);

                lightTerm += sample.BSDF * w / sample.LightPDF;
            }
            break;

        default:
            break;
        }
    }

    double nBSDFInv = nBSDFSamples > 0 ? 1.0 / ((double)nBSDFSamples) : 0.0;
    double nLightInv = nLightSamples > 0 ? 1.0 / ((double)nLightSamples) : 0.0;
    return nBSDFInv * bsdfTerm + nLightInv * lightTerm;
}

/**
 * [SurfaceIntegrator::SampleBSDF description]
 * @param  surfSamples [description]
 * @param  bounce      [description]
 * @param  maxBounces  [description]
 * @param  rayIn       [description]
 * @param  scn         [description]
 * @param  intsc       [description]
 * @return             [description]
 */

uint32_t SurfaceIntegrator::SampleBSDF(
    vector<SurfSample> &surfSamples,
    uint32_t bounce,
    uint32_t maxBounces,
    Ray rayIn,
    RTScene &scn,
    Intersection intsc,
    uint32_t &totalSamples
)
{
    const uint32_t requestedBSDFSamples = 4;

    vector<Ray> bsdfRays(requestedBSDFSamples);
    auto &mat = *scn.mats[intsc.mat];
    uint32_t numSamples = mat.GetBSDFSamples(requestedBSDFSamples, rayIn, intsc, bsdfRays);
    uint32_t nBSDFSamples = (uint32_t)bsdfRays.size();

    for (uint32_t i = 0; i < numSamples; i++)
    {
        Ray &ray = bsdfRays[i];

        Intersection nextIntsc;
        SurfSample curSample = { dvec3(0.0), 0.0, 0.0, BSDF_TYPE };
        curSample.BSDFPDF = mat.BSDFPDF(rayIn, ray, intsc);

        ray.org += bias * ray.dir;

        for (auto &lightKV : scn.lights)
        {
            Intersection lightIntsc;
            auto &light = *lightKV.second;

            light.Intersect(ray, lightIntsc);

            if (lightIntsc.t > 0.0)
            {
                curSample.LightPDF = light.GetLightPDF(ray, lightIntsc);
                break;
            }
        }

        scn.Intersect(ray, nextIntsc);

        if ((nextIntsc.t > bias))
        {
            dvec3 colorIn;

            if (nextIntsc.mat != 6)
            {
                colorIn = SampleSurface(
                    ray,
                    scn,
                    nextIntsc,
                    bounce + 1,
                    maxBounces
                );
            }

            curSample.BSDF = mat.EvalBSDF(ray, colorIn, intsc, rayIn);
        }

        surfSamples[totalSamples++] = curSample;
        ray.org -= bias * ray.dir;
    }

    return nBSDFSamples;
}

/**
 * [SurfaceIntegrator::SampleLightDistribution description]
 * @param  surfSamples [description]
 * @param  bounce      [description]
 * @param  maxBounces  [description]
 * @param  rayIn       [description]
 * @param  scn         [description]
 * @param  intsc       [description]
 * @return             [description]
 */

uint32_t SurfaceIntegrator::SampleLightDistribution(
    vector<SurfSample> &surfSamples,
    uint32_t bounce,
    uint32_t maxBounces,
    Ray rayIn,
    RTScene &scn,
    Intersection intsc,
    uint32_t &totalSamples
)
{
    uint32_t nLightSamples = 0;
    auto &mat = *scn.mats[intsc.mat];

    for (auto &lightKV : scn.lights)
    {
        const uint32_t numLightSamples = 8;

        vector<Ray> lightRays(numLightSamples);
        auto &light = *lightKV.second;

        light.GetLightSamples(8, rayIn, intsc, lightRays);
        nLightSamples += (uint32_t)lightRays.size();

        for (uint32_t i = 0; i < numLightSamples; i++)
        {
            Ray &ray = lightRays[i];

            Intersection nextIntsc;
            Intersection lightIntsc;
            SurfSample curSample = { dvec3(0.0), 0.0, 0.0, LIGHT_TYPE };

            ray.org += bias * ray.dir;

            light.Intersect(ray, lightIntsc);
            scn.Intersect(ray, nextIntsc);

            curSample.BSDFPDF = mat.BSDFPDF(rayIn, ray, intsc);
            curSample.LightPDF = light.GetLightPDF(ray, lightIntsc);

            if (abs(lightIntsc.t - nextIntsc.t) < bias)
            {
                double t = lightIntsc.t;
                dvec3 color = light.EvalEmission(ray, lightIntsc) / (t * t);
                curSample.BSDF = mat.EvalBSDF(ray, color, intsc, rayIn);
            }

            surfSamples[totalSamples++] = curSample;

            ray.org -= bias * ray.dir;
        }
    }

    return nLightSamples;
}

/**
 * [SurfaceIntegrator::NextVLightSet description]
 * @return [description]
 */

void SurfaceIntegrator::NextVLightSet()
{
    curVLightSet++;

    if (curVLightSet >= numVLightSets)
    {
        curVLightSet = 0;
    }
}

/**
 * [SurfaceIntegrator::SampleVirtualLights description]
 * @param  rayIn      [description]
 * @param  scn        [description]
 * @param  intsc      [description]
 * @param  bounce     [description]
 * @param  maxBounces [description]
 * @return            [description]
 */

dvec3 SurfaceIntegrator::SampleVirtualLights(
    Ray rayIn,
    RTScene &scn,
    Intersection intsc,
    uint32_t bounce,
    uint32_t maxBounces
)
{
    dvec3 virtColor = dvec3(0.0, 0.0, 0.0);

    if (vLightSetSize > 0)
    {
        Ray newRay;
        newRay.org = rayIn.org + (intsc.t * rayIn.dir);
        Intersection nextIntsc;
        NextVLightSet();

        for (uint32_t i = 0; i < scn.vLights[curVLightSet].size(); i++)
        {
            newRay.dir = normalize(scn.vLights[curVLightSet][i].pos - newRay.org);

            if (dot(newRay.dir, intsc.normal) > 0.0)
            {
                newRay.org += bias * newRay.dir;
                scn.Intersect(newRay, nextIntsc);
                newRay.org -= bias * newRay.dir;

                double t = nextIntsc.t;
                double dist = length(scn.vLights[curVLightSet][i].pos - newRay.org);

                if (t > 0.0 && abs(dist - t) < 2.0 * bias)
                {
                    double theta1 = dot(newRay.dir, intsc.normal);
                    double theta2 = dot(-newRay.dir, scn.vLights[curVLightSet][i].normal);
                    double g = theta1 * theta2 / (t * t);

                    if (g < 5.0)
                    {
                        virtColor += g * scn.vLights[curVLightSet][i].color;
                    }
                    else
                    {
                        double g2 = (g - 5.0) * t * t / theta2;
                        virtColor += (5.0 + g2) * scn.vLights[curVLightSet][i].color;
                    }
                }
            }
        }
    }

    return virtColor;
}