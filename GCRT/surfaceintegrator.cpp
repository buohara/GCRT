#include "surfaceintegrator.h"

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
    
}

/**
 * [SurfaceIntegrator::CalcDiffuse description]
 * @param  rayIn      [description]
 * @param  scn        [description]
 * @param  intsc      [description]
 * @param  bounce     [description]
 * @param  maxBounces [description]
 * @return            [description]
 */

dvec3 SurfaceIntegrator::CalcDiffuse(
    Ray rayIn,
    RTScene &scn,
    Intersection intsc,
    uint32_t bounce,
    uint32_t maxBounces
)
{
    uint32_t numSamples = numLightSamples + vLightSetSize;

    dvec3 diffColor = SampleDirectLights(
        rayIn,
        scn,
        intsc,
        bounce,
        maxBounces
    );

    dvec3 virtColor = SampleVirtualLights(
        rayIn,
        scn,
        intsc,
        bounce,
        maxBounces
    );

    return (2.0 * virtColor + diffColor) / ((double)numSamples);
}

/**
 * [SurfaceIntegrator::SampleDirectLights description]
 * @param  rayIn      [description]
 * @param  scn        [description]
 * @param  intsc      [description]
 * @param  bounce     [description]
 * @param  maxBounces [description]
 * @return            [description]
 */

dvec3 SurfaceIntegrator::SampleDirectLights(
    Ray rayIn,
    RTScene &scn,
    Intersection intsc,
    uint32_t bounce,
    uint32_t maxBounces
)
{
    Ray newRay;
    newRay.org = rayIn.org + (intsc.t * rayIn.dir);
    Intersection nextIntsc;
    dvec3 diffColor = dvec3(0.0, 0.0, 0.0);

    for (uint32_t i = 0; i < scn.lights.size(); i++)
    {
        auto pMat = scn.mats[scn.lights[i].mat];

        dvec3 emis = pMat->GetEmission(rayIn, intsc);
        dmat4 trans = translate(scn.lights[i].orgn);
        dmat4 scl = scale(dvec3(scn.lights[i].r));

        for (uint32_t j = 0; j < sphereSamples.size(); j++)
        {
            dmat4 rot = NextRotation();
            dvec3 sample = trans * rot * scl * sphereSamples[j];

            newRay.dir = normalize(sample - newRay.org);

            if (dot(newRay.dir, intsc.normal) > 0.0)
            {
                newRay.org += bias * newRay.dir;
                scn.Intersect(newRay, nextIntsc);
                newRay.org -= bias * newRay.dir;

                double t = nextIntsc.t;
                double dist = length(sample - newRay.org);

                if (nextIntsc.t > 0.0 && abs(dist - t) < 2.0 * bias)
                {
                    double theta1 = dot(newRay.dir, intsc.normal);
                    double theta2 = dot(-newRay.dir, nextIntsc.normal);
                    double g = theta1 * theta2 / (t * t);
                    diffColor += g * emis;
                }
            }
        }
    }

    return diffColor;
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