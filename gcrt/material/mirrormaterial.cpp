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

dvec3 RTMaterial::MirrorEvalBSDF(Ray sampleRay, dvec3 sampleColor, Intersection intsc, Ray camRay)
{
    return sampleColor;
}

/**
 * [MirrorGetBSDFSamples description]
 * @param numSamples [description]
 * @param rayIn      [description]
 * @param intsc      [description]
 * @param raysOut    [description]
 * @param weights    [description]
 */

uint32_t RTMaterial::MirrorGetBSDFSamples(uint32_t numSamples, Ray camRay, Intersection intsc, vector<Ray> &sampleRays)
{
    sampleRays[0].org = camRay.org + (intsc.t * camRay.dir);
    sampleRays[0].dir = normalize(reflect(camRay.dir, intsc.normal));
    return 1;
}