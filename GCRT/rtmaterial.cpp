#include "rtmaterial.h"

/**
 * [MatteMaterial::GetReflectance description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double MatteMaterial::GetReflectance(Ray ray, Intersection intsc)
{
    return 0.0;
}

/**
 * [MatteMaterial::GetTransmittance description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double MatteMaterial::GetTransmittance(Ray ray, Intersection intsc)
{
    return 0.0;
}

/**
 * [MatteMaterial::GetDiffuse description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double MatteMaterial::GetDiffuse(Ray ray, Intersection intsc)
{
    return 1.0;
}

/**
 * [MatteMaterial::GetReflectedRay description]
 * @param rayIn  [description]
 * @param intsc  [description]
 * @param rayOut [description]
 */

void MatteMaterial::GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut)
{
    return;
}

/**
 * [MatteMaterial::GetTransmittedRay description]
 * @param ray    [description]
 * @param intsc  [description]
 * @param rayOut [description]
 */

void MatteMaterial::GetTransmittedRay(Ray ray, Intersection intsc, Ray &rayOut)
{
    return;
}

/**
 * [MatteMaterial::GetEmission description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

dvec3 MatteMaterial::GetEmission(Ray ray, Intersection intsc)
{
    return dvec3(0.0, 0.0, 0.0);
}

/**
 * [MirrorMaterial::GetReflectance description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double MirrorMaterial::GetReflectance(Ray ray, Intersection intsc)
{
    return 1.0;
}

/**
 * [MirrorMaterial::GetTransmittance description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double MirrorMaterial::GetTransmittance(Ray ray, Intersection intsc)
{
    return 0.0;
}

/**
 * [MirrorMaterial::GetDiffuse description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double MirrorMaterial::GetDiffuse(Ray ray, Intersection intsc)
{
    return 0.0;
}

/**
 * [MirrorMaterial::GetReflectedRay description]
 * @param rayIn  [description]
 * @param intsc  [description]
 * @param rayOut [description]
 */

void MirrorMaterial::GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut)
{
    rayOut.org = rayIn.org + (intsc.t * rayIn.dir);
    rayOut.dir = normalize(reflect(rayIn.dir, intsc.normal));
}

/**
 * [MirrorMaterial::GetTransmittedRay description]
 * @param ray    [description]
 * @param intsc  [description]
 * @param rayOut [description]
 */

void MirrorMaterial::GetTransmittedRay(Ray ray, Intersection intsc, Ray &rayOut)
{
    return;
}

/**
 * [MirrorMaterial::GetEmission description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

dvec3 MirrorMaterial::GetEmission(Ray ray, Intersection intsc)
{
    return dvec3(0.0, 0.0, 0.0);
}

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
    double cost = sqrt(1.0 - sint * sint);

    if (sint > 1.0)
    {
        return 1.0;
    }

    double rpar = (etaTr * cosi - etaIn * cost) /
        (etaTr * cosi + etaIn * cost);

    double rperp = (etaIn * cosi - etaTr * cost) /
        (etaIn * cosi + etaTr * cost);

    return 0.5 * (rpar * rpar + rperp * rperp);
}

/**
 * [GlassMaterial::GetTransmittance description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double GlassMaterial::GetTransmittance(Ray ray, Intersection intsc)
{
    return 1.0 - GetReflectance(ray, intsc);
}

/**
 * [GlassMaterial::GetDiffuse description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double GlassMaterial::GetDiffuse(Ray ray, Intersection intsc)
{
    return 0.0;
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
    rayOut.dir = normalize(reflect(rayIn.dir, intsc.normal));
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
        rayOut.dir = normalize(refract(ray.dir, intsc.normal, etat / etai));
    }
    else
    {
        rayOut.dir = normalize(refract(ray.dir, -intsc.normal, etai / etat));
    }
}

/**
 * [GlassMaterial::GetEmission description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

dvec3 GlassMaterial::GetEmission(Ray ray, Intersection intsc)
{
    return dvec3(0.0, 0.0, 0.0);
}

/**
 * [LightMaterial::GetReflectance description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double LightMaterial::GetReflectance(Ray ray, Intersection intsc)
{
    return 0.0;
}

/**
 * [LightMaterial::GetTransmittance description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double LightMaterial::GetTransmittance(Ray ray, Intersection intsc)
{
    return 0.0;
}

/**
 * [LightMaterial::GetDiffuse description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

double LightMaterial::GetDiffuse(Ray ray, Intersection intsc)
{
    return 0.0;
}

/**
 * [LightMaterial::GetReflectedRay description]
 * @param rayIn  [description]
 * @param intsc  [description]
 * @param rayOut [description]
 */

void LightMaterial::GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut)
{
    return;
}

/**
 * [LightMaterial::GetTransmittedRay description]
 * @param ray    [description]
 * @param intsc  [description]
 * @param rayOut [description]
 */

void LightMaterial::GetTransmittedRay(Ray ray, Intersection intsc, Ray &rayOut)
{
    return;
}

/**
 * [LightMaterial::GetEmission description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

dvec3 LightMaterial::GetEmission(Ray ray, Intersection intsc)
{
    return lightColor;
}