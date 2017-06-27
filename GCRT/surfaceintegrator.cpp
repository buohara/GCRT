#include "surfaceintegrator.h"

/**
 * [PerturbVector description]
 * @param  cntr     [description]
 * @param  alphaMax [description]
 * @return          [description]
 */

dvec3 PerturbVector(dvec3 cntr, double alphaMax)
{
    dvec3 n = cntr;
    dvec3 t;

    t.x = -n.z;
    t.y = 0.0;
    t.z = n.x;

    t = normalize(t);

    dvec3 b = cross(n, t);
    mat3 tbn = mat3(t, b, n);

    dvec4 newVec4 = dvec4(0.0, 0.0, 1.0, 1.0);

    alphaMax = radians(alphaMax);

    double alphaRand = alphaMax * (double)rand() / (double)RAND_MAX;
    double thetaRand = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;

    mat4 tilt = rotate(alphaRand, dvec3(1.0, 0.0, 0.0));
    mat4 spin = rotate(thetaRand, dvec3(0.0, 0.0, 1.0));
    newVec4 = spin * tilt * newVec4;

    dvec3 newVec;
    newVec.x = newVec4.x;
    newVec.y = newVec4.y;
    newVec.z = newVec4.z;

    newVec = tbn * newVec;
    return newVec;
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
    newRay.org = ray.org + intsc.t * ray.dir;
    Intersection newIntsc;
    dvec3 outColor = dvec3(0.0, 0.0, 0.0);

    // Mirror material.

    if (intsc.mat.type == "Mirror")
    {       
        newRay.dir = normalize(reflect(ray.dir, intsc.normal));
        newIntsc = scn.Intersect(newRay);

        if (bounce == maxBounces)
        {
            if (newIntsc.t > 0.0 && newIntsc.mat.type == "Light")
            {
                double theta1 = dot(newRay.dir, intsc.normal);
                double theta2 = dot(-newRay.dir, newIntsc.normal);

                return (theta1 * theta2 * newIntsc.mat.lightColor) / 
                    (newIntsc.t * newIntsc.t);
            }
            else
            {
                return outColor;
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

            return outColor;
        }
    }

    // Glass material

    else if (intsc.mat.type == "Glass")
    {
        double eta;

        if (dot(ray.dir, intsc.normal) < 0.0)
        {
            eta = 0.7;
        }
        else
        {
            eta = 1.0 / 0.7;
        }

        newRay.dir = normalize(refract(ray.dir, intsc.normal, eta));
        newIntsc = scn.Intersect(newRay);

        if (bounce == maxBounces)
        {
            if (newIntsc.t > 0.0 && newIntsc.mat.type == "Light")
            {
                double theta1 = dot(newRay.dir, intsc.normal);
                double theta2 = dot(-newRay.dir, newIntsc.normal);
                double t = newIntsc.t;

                return (theta1 * theta2 * newIntsc.mat.lightColor) / (t * t);
            }
            else
            {
                return outColor;
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

            return outColor;
        }
    }
    
    // Matte/Lambert material.

    else if (intsc.mat.type == "Matte")
    {
        uint32_t numSamples = 0;

        for (uint32_t i = 0; i < scn.spheres.size(); i++)
        {
            if (scn.spheres[i].mat.type == "Light")
            {
                dvec3 cnt = scn.spheres[i].orgn;
                dvec3 cntDir = normalize(cnt - newRay.org);

                for (uint32_t j = 0; j < numRays; j++)
                {
                    newRay.dir = PerturbVector(cntDir, intsc.mat.maxAlpha);
                    newIntsc = scn.Intersect(newRay);

                    if (newIntsc.t > 0.0 &&
                        newIntsc.mat.name == scn.spheres[i].mat.name)
                    {
                        double theta1 = dot(newRay.dir, intsc.normal);
                        double theta2 = dot(-newRay.dir, newIntsc.normal);
                        double t = newIntsc.t;

                        outColor += (theta1 * theta2 * newIntsc.mat.lightColor * intsc.mat.kd) / 
                            (t * t);

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