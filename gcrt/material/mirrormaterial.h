#pragma once

#include "rtmaterial.h"

struct MirrorMaterial : RTMaterial
{
    dvec3 EvalBSDF(
        Ray rayIn,
        dvec3 colorIn,
        Intersection intsc,
        Ray rayOut
    );
    
    uint32_t GetBSDFSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut
    );
};