#pragma once

#include "rtmaterial.h"
#include "coshemisampler.h"

struct LambertMaterial : RTMaterial
{
    dvec3 kd;
    CosHemiSampler sampler;

    double BSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc);

    dvec3 EvalBSDF(
        Ray rayIn,
        dvec3 colorIn,
        Intersection intsc,
        Ray rayOut
    );

    void Init(uint32_t sampleSets, uint32_t setSize);

    uint32_t GetBSDFSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut
    );
};