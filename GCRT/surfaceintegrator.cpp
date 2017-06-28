#include "surfaceintegrator.h"

/**
 * [SurfaceIntegrator::GenerateSamplePoints description]
 * @param numSamples [description]
 */

void SurfaceIntegrator::GenerateSamplePoints(uint32_t numSamples)
{
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

    for (uint32_t i = 0; i < 4096; i++)
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
    static uint32_t idx;

    if (idx >= 4096)
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
    Ray ray,
    RTScene &scn,
    Intersection intsc,
    uint32_t bounce,
    uint32_t maxBounces
)
{
    Ray newRay;
    newRay.org = ray.org + (intsc.t * ray.dir);
    Intersection newIntsc;
    dvec3 outColor = dvec3(0.0, 0.0, 0.0);
    double bias = 0.0001;

    // Mirror material.

    if (intsc.mat.type == "Mirror")
    {       
        newRay.dir = normalize(reflect(ray.dir, intsc.normal));
        newRay.org += bias * newRay.dir;
        scn.Intersect(newRay, newIntsc);

        if (newIntsc.t > 0.0)
        {
            if (bounce == maxBounces)
            {
                if (newIntsc.mat.type == "Light")
                {
                    double theta = dot(newRay.dir, intsc.normal);
                    outColor = (theta * newIntsc.mat.lightColor) /
                        (newIntsc.t * newIntsc.t);
                }
            }

            else
            {
                outColor = SampleSurface(
                    newRay,
                    scn,
                    newIntsc,
                    bounce + 1,
                    maxBounces
                );
            }
        }

        return outColor;
    }

    // Glass material

    else if (intsc.mat.type == "Glass")
    {
        double eta;

        if (dot(ray.dir, intsc.normal) < 0.0)
        {
            eta = 1.1 / 1.0; 
            newRay.dir = normalize(refract(ray.dir, intsc.normal, eta));
            newRay.org += bias * newRay.dir;;
        }
        else
        {
            eta = 1.0 / 1.1;
            newRay.dir = normalize(refract(ray.dir, -intsc.normal, eta));
            newRay.org += bias * newRay.dir;
        }

        scn.Intersect(newRay, newIntsc);

        if (newIntsc.t > 0.0)
        {
            if (bounce == maxBounces)
            {
                if (newIntsc.mat.type == "Light")
                {
                    double theta = dot(newRay.dir, intsc.normal);
                    double t = newIntsc.t;
                    outColor = (theta * newIntsc.mat.lightColor) / (t * t);
                }
            }
            else
            {
                outColor = SampleSurface(
                    newRay,
                    scn,
                    newIntsc,
                    bounce + 1,
                    maxBounces
                );
            }
        }

        return outColor;
    }
    
    // Matte/Lambert material.

    else if (intsc.mat.type == "Matte")
    {
        uint32_t numSamples = 0;

        for (uint32_t i = 0; i < scn.spheres.size(); i++)
        {
            if (scn.spheres[i].mat.type == "Light")
            {
                dmat4 trans = translate(scn.spheres[i].orgn);
                dmat4 scl = scale(vec3(scn.spheres[i].r));

                for (uint32_t j = 0; j < sphereSamples.size(); j++)
                {
                    dmat4 rot = NextRotation();
                    dvec3 sample = trans * rot * scl * sphereSamples[j];

                    newRay.dir = normalize(sample - newRay.org);
                    newRay.org += bias * newRay.dir;

                    scn.Intersect(newRay, newIntsc);

                    if (newIntsc.t > 0.0 &&
                        newIntsc.mat.name == scn.spheres[i].mat.name)
                    {
                        double theta1 = dot(newRay.dir, intsc.normal);
                        double theta2 = dot(-newRay.dir, newIntsc.normal);
                        double t = newIntsc.t;

                        outColor += (theta1 * newIntsc.mat.lightColor * intsc.mat.kd) /
                            (t * t);

                        numSamples++;
                    }
                    
                    else if (bounce < maxBounces && newIntsc.mat.type == "Glass")
                    {
                        outColor += SampleSurface(
                            newRay,
                            scn,
                            newIntsc,
                            bounce + 1,
                            maxBounces
                        );

                        numSamples++;
                    }
                }
            }
        }

        if (numSamples > 1)
        {
            outColor /= (double)(numSamples);
        }

        return outColor;
    }

    else
    {
        double theta = dot(-ray.dir, intsc.normal);
        return theta * intsc.mat.lightColor / (intsc.t * intsc.t);
    }
}