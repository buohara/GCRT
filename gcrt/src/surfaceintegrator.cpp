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

    if (color.x == 0.0 && color.y == 0.0 && color.z == 0.0) return true;

    return false;
}

/**
 * DumpSamples For given surface intersection, dump samples used to estimate render
 * integral on that surface.
 *
 * @param bsdfSamples  Material BSDF samples used.
 * @param lightSamples Light samples used.
 */

void DumpSamples(vector<SurfSample> &bsdfSamples, vector<SurfSample> &lightSamples)
{
    printf("\nSurface Samples:\n\n");

    for (auto &sample : bsdfSamples)
    {
        printf(
            "BSDF: (%2.2f, %2.2f, %2.2f), BSDF PDF = %2.2f, Light PDF = %2.2f\n",
            sample.BSDF.x,
            sample.BSDF.y, 
            sample.BSDF.z,
            sample.BSDFPDF,
            sample.LightPDF
        );
    }

    for (auto &sample : lightSamples)
    {
        printf(
            "Light: (%2.2f, %2.2f, %2.2f), BSDF PDF = %2.2f, Light PDF = %2.2f\n",
            sample.BSDF.x,
            sample.BSDF.y,
            sample.BSDF.z,
            sample.BSDFPDF,
            sample.LightPDF
        );
    }
}

/**
 * SurfaceIntegrator::DumpPath For debugging. Print a ray path and the intersections
 * it has hit along the way.
 */

void SurfaceIntegrator::DumpPath()
{
    printf("\nRay Path Data\n\n");

    for (uint32_t i = 0; i < tls_pathDbgData.stackPtr; i++)
    {
        printf(
            "Ray %d: Origin = (%2.2f, %2.2f, %2.2f), Origin Mat = %s\n", 
            i,
            tls_pathDbgData.rayOrigins[i].x,
            tls_pathDbgData.rayOrigins[i].y,
            tls_pathDbgData.rayOrigins[i].z,
            "TODO: Fix SurfaceIntegrator::DumpPath"
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
    vector<SurfSample> bsdfSamples;
    vector<SurfSample> lightSamples;
    const uint32_t maxSampleAttempts = 10;
    
    auto &mat = scn.mats[intsc.mat];

    tls_pathDbgData.rayOrigins[tls_pathDbgData.stackPtr++] = rayIn.org;
    tls_pathDbgData.intscMaterials[tls_pathDbgData.stackPtr] = intsc.mat;

    if (bounce < maxBounces)
    {
        SampleBSDF(
            bsdfSamples,
            bounce,
            maxBounces,
            rayIn,
            scn,
            intsc
        );
    }

    dvec3 out = { 0.0, 0.0, 0.0 };

    if (bounce == maxBounces)
    {
        uint32_t sampleAttempts = 0;

        while (InvalidColor(out))
        {
            if (sampleAttempts == maxSampleAttempts) break;

            SampleLightDistribution(
                lightSamples,
                bounce,
                maxBounces,
                rayIn,
                scn,
                intsc
            );

            out = ApplyBalanceHeuristic(bsdfSamples, lightSamples);
            sampleAttempts++;
        }
    }
    else
    {
        SampleLightDistribution(
            lightSamples,
            bounce,
            maxBounces,
            rayIn,
            scn,
            intsc
        );

        out = ApplyBalanceHeuristic(bsdfSamples, lightSamples);
    }
    
    return out;
}

/**
 * SurfaceIntegrator::ApplyBalanceHeuristic Weight together different sets of
 * rendering equation samples obtained from different distributions (e.g., BSDF and
 * light distributions).
 *
 * @param  bsdfSamples   A list of BSDF samples.
 * @param  lightSamples  A list of light samples.
 *
 * @return               Color obtained by weighting samples from different distributions.
 */

dvec3 SurfaceIntegrator::ApplyBalanceHeuristic(
    vector<SurfSample> &bsdfSamples,
    vector<SurfSample> &lightSamples
)
{
    dvec3 bsdfTerm = dvec3(0.0);
    dvec3 lightTerm = dvec3(0.0);

    uint32_t nBSDFSamples   = (uint32_t)bsdfSamples.size();
    uint32_t nLightSamples  = (uint32_t)lightSamples.size();

    // Weight BSDF samples.

    for (uint32_t i = 0; i < nBSDFSamples; i++)
    {
        SurfSample &sample = bsdfSamples[i];

        if (sample.BSDFPDF == 0.0) bsdfTerm += sample.BSDF;
        else
        {
            double w = (double)nBSDFSamples * sample.BSDFPDF /
                ((double)nBSDFSamples * sample.BSDFPDF + (double)nLightSamples * sample.LightPDF);

            bsdfTerm += sample.BSDF * w / sample.BSDFPDF;
        }
    }

    // Weight light samples.

    for (uint32_t i = 0; i < nLightSamples; i++)
    {
        SurfSample &sample = lightSamples[i];

        if (sample.BSDFPDF == 0.0) continue;
        else
        {
            double w = (double)nLightSamples * sample.LightPDF /
                ((double)nBSDFSamples * sample.BSDFPDF + (double)nLightSamples * sample.LightPDF);

            lightTerm += sample.BSDF * w / sample.LightPDF;
        }
    }

    // Combine terms.

    double nBSDFInv = nBSDFSamples > 0 ? 1.0 / ((double)nBSDFSamples) : 0.0;
    double nLightInv = nLightSamples > 0 ? 1.0 / ((double)nLightSamples) : 0.0;
    
    dvec3 out = nBSDFInv * bsdfTerm + nLightInv * lightTerm;

    return out;
}

/**
 * SurfaceIntegrator::SampleBSDF Given a surface intersection, generate sample rays based on
 * its material BSDF, cast them into the scene, and collect the samples for use in a
 * Monte Carlo estimator.
 *
 * @param  surfSamples Array to hold collected samples.
 * @param  bounce      Number of bounces so far from camera.
 * @param  maxBounces  Max number of bounces for current ray path.
 * @param  rayIn       Ray coming from camera.
 * @param  scn         Ray tracing scene (into which samples rays are cast).
 * @param  intsc       Surface intersection data.
 */

void SurfaceIntegrator::SampleBSDF(
    vector<SurfSample> &surfSamples,
    uint32_t bounce,
    uint32_t maxBounces,
    Ray rayIn,
    RTScene &scn,
    Intersection intsc
)
{
    vector<Ray> bsdfRays(numBSDFSamples);
    auto &mat = scn.mats[intsc.mat];
    uint32_t numSamples = mat.GetBSDFSamples(numBSDFSamples, rayIn, intsc, bsdfRays);
    uint32_t sampleCnt = 0;
    surfSamples.resize(numSamples);

    for (uint32_t i = 0; i < numSamples; i++)
    {
        Ray &ray = bsdfRays[i];
        Intersection nextIntsc;
        ray.org += bias * ray.dir;
        scn.Intersect(ray, nextIntsc);

        if ((nextIntsc.t > bias) && nextIntsc.isLight) continue;

        SurfSample curSample = { dvec3(0.0), 0.0, 0.0 };
        curSample.BSDFPDF = mat.BSDFPDF(rayIn, ray, intsc);

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

        if ((nextIntsc.t > bias))
        {
            dvec3 colorIn = SampleSurface(
                ray,
                scn,
                nextIntsc,
                bounce + 1,
                maxBounces
            );

            curSample.BSDF = mat.EvalBSDF(ray, colorIn, intsc, rayIn);
        }

        surfSamples[sampleCnt++] = curSample;
        ray.org -= bias * ray.dir;
    }

    surfSamples.resize(sampleCnt);
}

/**
 * SurfaceIntegrator::SampleLightDistribution For a given surface intersection,
 * generate sample rays toward light sources, cast sample rays into scene, and collect
 * samples for use in a Monte Carlo estimator.
 *
 * @param  surfSamples Array to hold light samples.
 * @param  bounce      Current bounce number along ray path.
 * @param  maxBounces  Maximum number of allowed bounces along this ray path.
 * @param  rayIn       Ray coming from camera.
 * @param  scn         Ray-tracing scene (into which samples rays are cast).
 * @param  intsc       Surface intersection data.
 */

void SurfaceIntegrator::SampleLightDistribution(
    vector<SurfSample> &surfSamples,
    uint32_t bounce,
    uint32_t maxBounces,
    Ray rayIn,
    RTScene &scn,
    Intersection intsc
)
{
    auto &mat = scn.mats[intsc.mat];
    uint32_t curLightSample = 0;

    surfSamples.resize(numLightSamples * scn.lights.size());

    for (auto &lightKV : scn.lights)
    {
        vector<Ray> lightRays(numLightSamples);
        auto &light = *lightKV.second;

        light.GetLightSamples(numLightSamples, rayIn, intsc, lightRays);

        for (uint32_t i = 0; i < numLightSamples; i++)
        {
            Ray &ray = lightRays[i];

            Intersection nextIntsc;
            Intersection lightIntsc;
            SurfSample curSample = { dvec3(0.0), 0.0, 0.0 };

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
            else continue;

            surfSamples[curLightSample++] = curSample;
            ray.org -= bias * ray.dir;
        }
    }

    surfSamples.resize(curLightSample++);
}

/**
 * SurfaceIntegrator::NextVLightSet For bidirectional path tracing, get light nodes
 * set deposited during preprocess when casting rays from light sources.
 */

void SurfaceIntegrator::NextVLightSet()
{
    curVLightSet++;
    if (curVLightSet >= numVLightSets) curVLightSet = 0;
}

/**
 * SurfaceIntegrator::SampleVirtualLights For bidirectional path tracing, sample
 * virtual lights deposited from rays shot from light sources.
 *
 * @param  rayIn      Ray coming from camera.
 * @param  scn        Scene to casts rays into.
 * @param  intsc      Surface intersection data.
 * @param  bounce     Current bounce number along camera path.
 * @param  maxBounces Max number of bounces allowed along this path.
 * @return            Illumination at current intersection from indirect/virtual lights.
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

                    if (g < 5.0) virtColor += g * scn.vLights[curVLightSet][i].color;
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