#include "surfaceintegrator.h"

/**
 * [SurfaceIntegrator::GenerateSamplePoints description]
 * @param numSamples [description]
 */

void SurfaceIntegrator::GenerateSphereSamples(uint32_t numSamples)
{
    idx = 0;

    dvec4 sample;
    sample.x = 0.0;
    sample.y = 0.0;
    sample.z = 0.0;
    sample.w = 1.0;

    sphereSamples.push_back(sample);

    for (uint32_t i = 1; i < numSamples; i++)
    {
        double theta = pi<double>() * (double)rand() / (double)RAND_MAX;
        double phi = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;

        sample.x = sin(theta) * cos(phi);
        sample.y = sin(theta) * sin(phi);
        sample.z = cos(theta);
        sample.w = 1.0;

        sphereSamples.push_back(sample);
    }

    for (uint32_t i = 0; i < 1024; i++)
    {
        double rotRand = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;
        dmat4 rot = rotate(rotRand, dvec3(0.0, 0.0, 1.0));
        randomRotations.push_back(rot);
    }
}

/**
 * [SurfaceIntegrator::NextRotation description]
 * @return [description]
 */

dmat4 SurfaceIntegrator::NextRotation()
{
    if (idx >= 1024)
    {
        idx = 0;
    }

    return randomRotations[idx++];
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
    dvec3 outColor = dvec3(0.0, 0.0, 0.0);

    if (intsc.mat->krefl > 0.0)
    {
        outColor += CalcReflectance(
            rayIn,
            scn,
            intsc,
            bounce,
            maxBounces
        );
    }

    if (intsc.mat->ktrans > 0.0)
    {
        outColor += CalcTransmittance(
            rayIn,
            scn,
            intsc,
            bounce,
            maxBounces
        );
    }

    if (intsc.mat->kdiff > 0.0)
    {
        outColor += CalcDiffuse(
            rayIn,
            scn,
            intsc,
            bounce,
            maxBounces
        );
    }

    if (intsc.mat->emissive == true)
    {
        outColor += intsc.mat->ke;
    }

    return outColor;
}

/**
 * [SurfaceIntegrator::CalcReflectance description]
 * @param  rayIn      [description]
 * @param  scn        [description]
 * @param  intsc      [description]
 * @param  bounce     [description]
 * @param  maxBounces [description]
 * @return            [description]
 */

dvec3 SurfaceIntegrator::CalcReflectance(
    Ray rayIn,
    RTScene &scn,
    Intersection intsc,
    uint32_t bounce,
    uint32_t maxBounces
)
{
    Ray rayOut;
    rayOut.org = rayIn.org + (intsc.t * rayIn.dir);
    Intersection nextIntsc;
    dvec3 refColor = dvec3(0.0, 0.0, 0.0);

    if (intsc.mat->krefl > 0.0)
    {
        rayOut.dir = normalize(reflect(rayIn.dir, intsc.normal));
        rayOut.org += bias * rayOut.dir;
        scn.Intersect(rayOut, nextIntsc);
        rayOut.org -= bias * rayOut.dir;

        if (nextIntsc.t > 0.0)
        {
            if (bounce == maxBounces)
            {
                if (nextIntsc.mat->emissive)
                {
                    double theta = dot(rayOut.dir, intsc.normal);
                    refColor = (theta * nextIntsc.mat->ke) /
                        (nextIntsc.t * nextIntsc.t);
                }
            }

            else
            {
                refColor = SampleSurface(
                    rayOut,
                    scn,
                    nextIntsc,
                    bounce + 1,
                    maxBounces
                );
            }
        }
    }

    return refColor;
}

/**
 * [CalcTransmittance description]
 * @param  rayIn      [description]
 * @param  scn        [description]
 * @param  intsc      [description]
 * @param  bounce     [description]
 * @param  maxBounces [description]
 * @return            [description]
 */

dvec3 SurfaceIntegrator::CalcTransmittance(
    Ray rayIn,
    RTScene &scn,
    Intersection intsc,
    uint32_t bounce,
    uint32_t maxBounces
)
{
    Ray rayOut;
    rayOut.org = rayIn.org + (intsc.t * rayIn.dir);
    Intersection nextIntsc;
    dvec3 transColor = dvec3(0.0, 0.0, 0.0);

    if (intsc.mat->ktrans > 0.0)
    {
        double eta;

        if (dot(rayIn.dir, intsc.normal) < 0.0)
        {
            eta = 1.1 / 1.0;
            rayOut.dir = normalize(refract(rayIn.dir, intsc.normal, eta));
            rayOut.org += bias * rayOut.dir;;
        }
        else
        {
            eta = 1.0 / 1.1;
            rayOut.dir = normalize(refract(rayIn.dir, -intsc.normal, eta));
            rayOut.org += bias * rayOut.dir;
        }

        scn.Intersect(rayOut, nextIntsc);
        rayOut.org -= bias * rayOut.dir;

        if (nextIntsc.t > 0.0)
        {
            if (bounce == maxBounces)
            {
                if (nextIntsc.mat->emissive)
                {
                    double theta = dot(rayOut.dir, intsc.normal);
                    double t = nextIntsc.t;
                    transColor = (theta * nextIntsc.mat->ke) / (t * t);
                }
            }
            else
            {
                transColor = SampleSurface(
                    rayOut,
                    scn,
                    nextIntsc,
                    bounce + 1,
                    maxBounces
                );
            }
        }
    }

    return transColor;
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
    Ray rayOut;
    rayOut.org = rayIn.org + (intsc.t * rayIn.dir);
    Intersection nextIntsc;
    dvec3 diffColor = dvec3(0.0, 0.0, 0.0);

    if (intsc.mat->kdiff > 0.0)
    {
        uint32_t numSamples = sphereSamples.size();

        for (uint32_t i = 0; i < scn.spheres.size(); i++)
        {
            if (scn.spheres[i].mat->emissive)
            {
                dmat4 trans = translate(scn.spheres[i].orgn);
                dmat4 scl = scale(vec3(scn.spheres[i].r));

                for (uint32_t j = 0; j < sphereSamples.size(); j++)
                {
                    dmat4 rot = NextRotation();
                    dvec3 sample = trans * rot * scl * sphereSamples[j];

                    rayOut.dir = normalize(sample - rayOut.org);
                    rayOut.org += bias * rayOut.dir;
                    scn.Intersect(rayOut, nextIntsc);
                    rayOut.org -= bias * rayOut.dir;
                    double t = nextIntsc.t;

                    if (nextIntsc.t > 0.0 &&
                        nextIntsc.mat->name == scn.spheres[i].mat->name)
                    {
                        double theta1 = dot(rayOut.dir, intsc.normal);
                        double theta2 = dot(-rayOut.dir, nextIntsc.normal);

                        diffColor += (theta1 * nextIntsc.mat->ke * intsc.mat->kd) /
                            (t * t);
                    }

                    else if (
                        bounce < maxBounces &&
                        (nextIntsc.mat->ktrans > 0.0)
                    )
                    {
                        diffColor += SampleSurface(
                            rayOut,
                            scn,
                            nextIntsc,
                            bounce + 1,
                            maxBounces
                        );
                    }
                }
            }
        }

        if (numSamples > 1)
        {
            diffColor /= (double)(numSamples);
        }
    }

    return diffColor;
}