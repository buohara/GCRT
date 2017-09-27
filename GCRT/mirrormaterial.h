#pragma once

#include "rtmaterial.h"

struct MirrorMaterial : RTMaterial
{
    dvec3 EvalBDRF(Ray rayIn, dvec3 colorIn, Ray rayOut);
    
    void GetSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut,
        vector<double> &weights
    );
};