#include "rtmaterial.h"

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
 * [LightMaterial::GetEmission description]
 * @param  ray   [description]
 * @param  intsc [description]
 * @return       [description]
 */

dvec3 LightMaterial::GetEmission(Ray ray, Intersection intsc)
{
    return lightColor;
}

/**
 * [TexMaterial::GetDiffuseColor description]
 * @return       [description]
 */

dvec3 TexMaterial::GetDiffuseColor(Ray ray, Intersection intsc)
{
    return diffTex.Sample(intsc.uv);
}

/**
 * [TexMaterial::Load description]
 * @param diffTexFile [description]
 * @param normTexFile [description]
 */

void TexMaterial::Load(string diffTexFile, string normTexFile)
{
    diffTex.Load(diffTexFile);
    normTex.Load(normTexFile);
}

/**
 * [TexMaterial::PerturbNormal description]
 * @param intsc  [description]
 * @param normal [description]
 */

void TexMaterial::PerturbNormal(Intersection &intsc)
{
    dvec3 nPerturb  = normTex.Sample(intsc.uv);
    dvec3 bitan     = cross(intsc.normal, intsc.tan);
    dmat3 tbn       = dmat3(intsc.tan, bitan, intsc.normal);
    intsc.normal    = tbn * intsc.normal;
}