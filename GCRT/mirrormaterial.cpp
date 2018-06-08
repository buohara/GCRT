#include "mirrormaterial.h"

/**
 * [MirrorMaterial::EvalBSDF description]
 * @param  rayIn   [description]
 * @param  intsc   [description]
 * @param  colorIn [description]
 * @param  rayOut  [description]
 * @return         [description]
 */

dvec3 MirrorMaterial::EvalBSDF(
    Ray rayIn,
    dvec3 colorIn,
    Intersection intsc,
    Ray rayOut
)
{
    return colorIn;
}

/**
 * [MirrorMaterial::GetBSDFSamples description]
 * @param numSamples [description]
 * @param rayIn      [description]
 * @param intsc      [description]
 * @param raysOut    [description]
 * @param weights    [description]
 */

uint32_t MirrorMaterial::GetBSDFSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut
)
{
    raysOut[0].org = rayIn.org + (intsc.t * rayIn.dir);
    raysOut[0].dir = normalize(reflect(rayIn.dir, intsc.normal));

    return 1;
}