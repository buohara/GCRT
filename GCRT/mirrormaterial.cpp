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
    Intersection intsc,
    dvec3 colorIn,
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

void MirrorMaterial::GetBSDFSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut
)
{
    Ray rayOut;
    rayOut.org = rayIn.org + (intsc.t * rayIn.dir);
    rayOut.dir = normalize(reflect(rayIn.dir, intsc.normal));
    raysOut.push_back(rayOut);
}