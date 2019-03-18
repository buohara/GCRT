#pragma once

#include "rtmaterial.h"
#include "coshemisampler.h"

struct BlinnMaterial : RTMaterial
{
    double ks;

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

    double BSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc);
};