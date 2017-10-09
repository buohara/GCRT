#include "glassmaterial.h"

/**
* [GlassMaterial::GetReflectance description]
* @param  ray   [description]
* @param  intsc [description]
* @return       [description]
*/

double GlassMaterial::GetReflectance(Ray ray, Intersection intsc)
{
    double etaIn;
    double etaTr;
    dvec3 inc;
    dvec3 norm;

    if (dot(ray.dir, intsc.normal) < 0.0)
    {
        inc = normalize(-ray.dir);
        norm = normalize(intsc.normal);
        etaIn = etai;
        etaTr = etat;
    }
    else
    {
        inc = normalize(-ray.dir);
        norm = normalize(-intsc.normal);
        etaIn = etat;
        etaTr = etai;
    }

    double cosi = dot(inc, norm);
    double sini = sqrt(1.0 - cosi * cosi);
    double sint = etaIn * sini / etaTr;

    if (sint > 1.0)
    {
        return 1.0;
    }

    double cost = sqrt(1.0 - sint * sint);

    double rpar = (etaTr * cosi - etaIn * cost) /
        (etaTr * cosi + etaIn * cost);

    double rperp = (etaIn * cosi - etaTr * cost) /
        (etaIn * cosi + etaTr * cost);

    return 0.5 * (rpar * rpar + rperp * rperp);
}

/**
* [GlassMaterial::GetReflectedRay description]
* @param rayIn  [description]
* @param intsc  [description]
* @param rayOut [description]
*/

void GlassMaterial::GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut)
{
    rayOut.org = rayIn.org + (intsc.t * rayIn.dir);

    if (dot(rayIn.dir, intsc.normal) < 0.0)
    {
        rayOut.dir = normalize(reflect(rayIn.dir, intsc.normal));
    }
    else
    {
        rayOut.dir = normalize(reflect(rayIn.dir, -intsc.normal));
    }
}

/**
* [GlassMaterial::GetTransmittedRay description]
* @param ray    [description]
* @param intsc  [description]
* @param rayOut [description]
*/

void GlassMaterial::GetTransmittedRay(Ray ray, Intersection intsc, Ray &rayOut)
{
    rayOut.org = ray.org + (intsc.t * ray.dir);

    if (dot(ray.dir, intsc.normal) < 0.0)
    {
        rayOut.dir = normalize(refract(ray.dir, intsc.normal, etai / etat));
    }
    else
    {
        rayOut.dir = normalize(refract(ray.dir, -intsc.normal, etat / etai));
    }
}

/**
 * [GlassMaterial::EvalBDRF description]
 * @param  rayIn   [description]
 * @param  colorIn [description]
 * @param  rayOut  [description]
 * @return         [description]
 */

dvec3 GlassMaterial::EvalBSDF(
    Ray rayIn,
    dvec3 colorIn,
    Intersection intsc,
    Ray rayOut
)
{
    return colorIn;
}

/**
 * [GlassMaterial::GetSamples description]
 * @param rayIn   [description]
 * @param intsc   [description]
 * @param raysOut [description]
 * @param weights [description]
 */

void GlassMaterial::GetBSDFSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut
)
{
    Ray rayReflect;
    Ray rayTransmit;
    double reflectWt = GetReflectance(rayIn, intsc);
    double transWt = 1.0 - reflectWt;

    GetReflectedRay(rayIn, intsc, rayReflect);
    GetTransmittedRay(rayIn, intsc, rayTransmit);

    raysOut.push_back(rayReflect);
    raysOut.push_back(rayTransmit);
}