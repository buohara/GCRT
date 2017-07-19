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
    shared_ptr<RTMaterial> pMat = scn.mats[intsc.mat];

    // Emissive

    outColor += pMat->GetEmission(rayIn, intsc);

    if (bounce == maxBounces)
    {
        return outColor;
    }

    double refl = pMat->GetReflectance(rayIn, intsc);
    double trans = pMat->GetTransmittance(rayIn, intsc);
    double diff = pMat->GetDiffuse(rayIn, intsc);

    // Reflection

    if (refl > bias)
    {
        Ray newRay;
        Intersection nextIntsc;

        pMat->GetReflectedRay(rayIn, intsc, newRay);
        newRay.org += bias * newRay.dir;
        scn.Intersect(newRay, nextIntsc);
        newRay.org -= bias * newRay.dir;

        double t = nextIntsc.t;

        if (t > bias)
        {
            dvec3 reflClr = dvec3(0.0, 0.0, 0.0);

            reflClr += SampleSurface(
                newRay,
                scn,
                nextIntsc,
                bounce + 1,
                maxBounces
            );

            outColor += refl * reflClr;
        }
    }

    // Transmission

    if (trans > bias)
    {
        Ray newRay;
        Intersection nextIntsc;

        pMat->GetTransmittedRay(rayIn, intsc, newRay);
        newRay.org += bias * rayIn.dir;
        scn.Intersect(newRay, nextIntsc);
        newRay.org -= bias * rayIn.dir;

        double t = nextIntsc.t;

        if (t > bias)
        {
            dvec3 transClr = dvec3(0.0, 0.0, 0.0);

            transClr += SampleSurface(
                newRay,
                scn,
                nextIntsc,
                bounce + 1,
                maxBounces
            );

            outColor += trans * transClr;
        }
    }

    // Diffuse

    if (diff > bias)
    {
        dvec3 diffColor = pMat->GetDiffuseColor();
        dvec3 diffColorIn;

        diffColorIn = CalcDiffuse(
            rayIn,
            scn,
            intsc,
            bounce,
            maxBounces
        );

        outColor += diff * diffColor * diffColorIn;
    }

    return outColor;
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
    Ray newRay;
    newRay.org = rayIn.org + (intsc.t * rayIn.dir);
    Intersection nextIntsc;
    dvec3 diffColor = dvec3(0.0, 0.0, 0.0);

    uint32_t numSamples = (uint32_t)sphereSamples.size();

    for (uint32_t i = 0; i < scn.spheres.size(); i++)
    {
        shared_ptr<RTMaterial> pMat = scn.mats[scn.spheres[i].mat];
        dvec3 emis = pMat->GetEmission(rayIn, intsc);

        if (emis.x > 0.0 || emis.y > 0.0 || emis.z > 0.0)
        {
            dmat4 trans = translate(scn.spheres[i].orgn);
            dmat4 scl = scale(dvec3(scn.spheres[i].r));

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

                    if (nextIntsc.t > 0.0 && abs(dist - t) < 2 * bias)
                    {
                        double theta = dot(newRay.dir, intsc.normal);
                        diffColor += 
                            theta * scn.mats[nextIntsc.mat]->GetEmission(newRay, nextIntsc) / (t * t);
                    }
                }
            }
        }
    }

    if (numSamples > 0)
    {
        diffColor /= (double)(numSamples);
    }

    return diffColor;
}