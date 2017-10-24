#include "surfaceintegrator.h"

/**
 * [SurfaceIntegrator::SampleSurface description]
 * @param  ray        [description]
 * @param  intsc      [description]
 * @param  bounce     [description]
 * @param  maxBounces [description]
 * @return            [description]
 */

dvec3 SurfaceIntegrator::SampleSurface(
    Ray rayIn,
    RTScene &scn,
    Intersection intsc,
    uint32_t bounce,
    uint32_t maxBounces
)
{
    vector<SurfSample> surfSamples;
    auto &mat = *scn.mats[intsc.mat];
    uint32_t nBSDFSamples = 0;
    uint32_t nLightSamples = 0;

    if (bounce < maxBounces)
    {
        nBSDFSamples = SampleBSDF(
            surfSamples,
            bounce,
            maxBounces,
            rayIn,
            scn,
            intsc
        );
    }

    nLightSamples = SampleLightDistribution(
        surfSamples,
        bounce,
        maxBounces,
        rayIn,
        scn,
        intsc
    );

    dvec3 out = ApplyBalanceHeuristic(surfSamples, nBSDFSamples, nLightSamples);
    return out;
}

/**
 * [SurfaceIntegrator::ApplyBalanceHeuristic description]
 * @param  surfSamples   [description]
 * @param  nBSDFSamples  [description]
 * @param  nLightSamples [description]
 * @return               [description]
 */

dvec3 SurfaceIntegrator::ApplyBalanceHeuristic(
    vector<SurfSample> &surfSamples,
    uint32_t nBSDFSamples,
    uint32_t nLightSamples
)
{
    dvec3 bsdfTerm = dvec3(0.0);
    dvec3 lightTerm = dvec3(0.0);

    for (auto sample : surfSamples)
    {
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
    Intersection intsc
)
{
    vector<Ray> bsdfRays;
    auto &mat = *scn.mats[intsc.mat];
    mat.GetBSDFSamples(4, rayIn, intsc, bsdfRays);
    uint32_t nBSDFSamples = bsdfRays.size();

    for (auto &ray : bsdfRays)
    {
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

            if (nextIntsc.mat != "Light")
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

        surfSamples.push_back(curSample);
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
    Intersection intsc
)
{
    uint32_t nLightSamples = 0;
    auto &mat = *scn.mats[intsc.mat];

    for (auto &lightKV : scn.lights)
    {
        vector<Ray> lightRays;
        auto &light = *lightKV.second;

        light.GetLightSamples(8, rayIn, intsc, lightRays);
        nLightSamples += lightRays.size();

        for (auto &ray : lightRays)
        {
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

            surfSamples.push_back(curSample);

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