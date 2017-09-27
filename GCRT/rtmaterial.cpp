#include "rtmaterial.h"

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
    intsc.normal    = tbn * nPerturb;
}