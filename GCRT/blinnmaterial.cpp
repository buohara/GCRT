#include "BlinnMaterial.h"

/**
 * [BlinnMaterial::EvalBSDF description]
 * @param  rayIn   [description]
 * @param  colorIn [description]
 * @param  intsc   [description]
 * @param  rayOut  [description]
 * @return         [description]
 */

dvec3 BlinnMaterial::EvalBSDF(
    Ray rayIn,
    dvec3 colorIn,
    Intersection intsc,
    Ray rayOut
)
{
    dvec3 r = normalize(reflect(-rayIn.dir, intsc.normal));
    double spec = dot(r, -rayOut.dir);
    spec = pow(spec, ks + 1.0);
    return 0.3 * spec * dvec3(2.0, 2.0, 2.0) * colorIn;
}

/**
 * [BlinnMaterial::GetBSDFSamples description]
 * @param numSamples [description]
 * @param rayIn      [description]
 * @param intsc      [description]
 * @param raysOut    [description]
 */

void BlinnMaterial::GetBSDFSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut
)
{
    dvec3 org = rayIn.org + intsc.t * rayIn.dir;
    dvec3 bitan = cross(intsc.normal, intsc.tan);
    dmat3 tbn = dmat3(intsc.tan, bitan, intsc.normal);

    for (uint32_t i = 0; i < numSamples; i++)
    {
        double u1 = (double)rand() / (double)RAND_MAX;
        double u2 = (double)rand() / (double)RAND_MAX;

        double cosTheta = pow(u1, 1.0 / (ks + 1.0));
        double sinTheta = sqrt(1.0 - cosTheta * cosTheta);
        double phi = 2.0 * pi<double>() * u2;

        dvec3 halfDir = dvec3(0.0);
        halfDir.x = sinTheta * cos(phi);
        halfDir.y = sinTheta * sin(phi);
        halfDir.z = cosTheta;
        halfDir = tbn * halfDir;

        dvec3 refl = normalize(reflect(rayIn.dir, intsc.normal));

        if (dot(halfDir, intsc.normal) < 0.0)
        {
            halfDir = -halfDir;
        }

        dvec3 newDir = rayIn.dir + 2.0 * dot(-rayIn.dir, halfDir) * halfDir;

        Ray newRay;
        newRay.dir = newDir;
        newRay.org = org;

        raysOut.push_back(newRay);
    }
}

/**
 * [BlinnMaterial::BSDFPDF description]
 * @param  rayIn  [description]
 * @param  rayOut [description]
 * @param  intsc  [description]
 * @return        [description]
 */

double BlinnMaterial::BSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc)
{
    dvec3 refl      = normalize(reflect(-rayOut.dir, intsc.normal));
    double cosAlpha = dot(refl, -rayIn.dir);
    cosAlpha        = pow(cosAlpha, ks + 1.0);
    double out      = (ks + 1.0) * cosAlpha / (8.0 * pi<double>());
    return out;
}