#include "rtmaterial.h"

RTMaterial::RTMaterial(MatType type, string name, double etai, double etat) :
    type(type), name(name), etai(etai), etat(etat)
{
    assert(type == FRESNEL);
}

/**
 * RTMaterial::GetReflectance Get amount of incoming ray reflected off
 * surface based on indices of refraction.
 *
 * @param  ray   Incoming ray.
 * @param  intsc Surface intersection data.
 * @return       Percent of incoming ray reflected.
 */

double RTMaterial::GetReflectance(Ray ray, Intersection intsc)
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
 * RTMaterial::GetReflectedRay Get ray reflected off surface for a given
 * input ray (reflected about the surface normal).
 *
 * @param rayIn  Incoming ray.
 * @param intsc  Surface intersection data.
 * @param rayOut Reflected ray.
 */

void RTMaterial::GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut)
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
 * RTMaterial::GetTransmittedRay Get ray transmitted through surface
 * for given incoming ray using Snell's law.
 *
 * @param ray    Incoming ray.
 * @param intsc  Surface intersection data.
 * @param rayOut Transmitted ray.
 */

void RTMaterial::GetTransmittedRay(Ray ray, Intersection intsc, Ray &rayOut)
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
 * RTMaterial::EvalBSDF Evaluate surface BSDF for given input and
 * output rays.
 *
 * @param  rayOut  Outgoing ray (going away from camera).
 * @param  colorIn Color returned along outgoing ray.
 * @param  intsc   Surface intersection data.
 * @param  rayIn   Incoming ray (coming from camera).
 * @return         BSDF value.
 */

dvec3 RTMaterial::FresnelEvalBSDF(
    Ray rayIn,
    dvec3 colorIn,
    Intersection intsc,
    Ray rayOut
)
{
    double reflectWt = GetReflectance(rayOut, intsc);

    if (dot(rayIn.dir, intsc.normal) > 0.0)
    {
        return 2.0 * reflectWt * colorIn;
    }
    else
    {
        double transWt = 1.0 - reflectWt;
        return 2.0 * transWt * colorIn;
    }
}

/**
 * RTMaterial::GetBSDFSamples Generate rays samples for MC estimator.
 *
 * @param numSamples Number of samples requested.
 * @param rayIn      Ray coming from camera.
 * @param intsc      Surface intersection data.
 * @param raysOut    Out sample rays (going away from camera).
 *
 * @return           Number of samples generated (some materials, like perfect mirrors,
 *                   only generate one output ray).
 */

uint32_t RTMaterial::FresnelGetBSDFSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut
)
{
    Ray rayReflect;
    Ray rayTransmit;
    
    GetReflectedRay(rayIn, intsc, rayReflect);
    GetTransmittedRay(rayIn, intsc, rayTransmit);

    raysOut[0] = rayReflect;
    raysOut[0] = rayTransmit;

    return 1;
}