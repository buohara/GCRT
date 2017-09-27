#pragma once

#include "rtmaterial.h"

struct MirrorMaterial : RTMaterial
{
    dvec3 EvalBSDF(
        Ray rayIn,
        Intersection intsc,
        dvec3 colorIn,
        Ray rayOut
    );
    
    void GetBSDFSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut,
        vector<double> &weights
    );
};