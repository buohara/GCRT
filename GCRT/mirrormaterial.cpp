#include "mirrormaterial.h"

dvec3 MirrorMaterial::EvalBDRF(Ray rayIn, dvec3 colorIn, Ray rayOut)
{
    return colorIn;
}

void MirrorMaterial::GetSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut,
    vector<double> &weights
)
{
    Ray rayOut;
    rayOut.org = rayIn.org + (intsc.t * rayIn.dir);
    rayOut.dir = normalize(reflect(rayIn.dir, intsc.normal));

    raysOut.push_back(rayOut);
    weights.push_back(1.0);
}