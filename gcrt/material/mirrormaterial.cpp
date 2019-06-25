#include "rtmaterial.h"

/**
 * RTMaterial - Constructor for mirror material.
 */

RTMaterial::RTMaterial(MatType type, string name) : type(type), name(name)
{
    assert(type == MIRROR);
}

/**
 * [MirrorEvalBSDF description]
 * @param  rayIn   [description]
 * @param  intsc   [description]
 * @param  colorIn [description]
 * @param  rayOut  [description]
 * @return         [description]
 */

dvec3 RTMaterial::MirrorEvalBSDF(Ray rayIn, dvec3 colorIn, Intersection intsc, Ray rayOut)
{
    return colorIn;
}

/**
 * [MirrorGetBSDFSamples description]
 * @param numSamples [description]
 * @param rayIn      [description]
 * @param intsc      [description]
 * @param raysOut    [description]
 * @param weights    [description]
 */

uint32_t RTMaterial::MirrorGetBSDFSamples(uint32_t numSamples, Ray rayIn, Intersection intsc, vector<Ray> &raysOut)
{
    raysOut[0].org = rayIn.org + (intsc.t * rayIn.dir);
    raysOut[0].dir = normalize(reflect(rayIn.dir, intsc.normal));
    return 1;
}