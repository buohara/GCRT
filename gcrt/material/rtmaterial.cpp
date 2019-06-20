#include "rtmaterial.h"

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

string RTMaterial::GetName(MaterialType mat)
{
    switch (mat)
    {
    case MIRROR:
        return string("MIRROR");
    case GLASS:
        return string("GLASS");
    case GREEN_MATTE:
        return string("GREEN_MATTE");
    case RED_MATTE:
        return string("RED_MATTE");
    case WHITE_MATTE:
        return string("WHITE_MATTE");
    case METAL:
        return string("METAL");
    case CAMERA:
        return string("CAMERA");
    default:
        return string("UNKNOWN");
    }
}