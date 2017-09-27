#pragma once

#include "GCRT.h"
#include "ray.h"

using namespace std;
using namespace glm;

struct Light
{
    dvec3 pwr;

    virtual void GetLightSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut,
        vector<double> &weights
    )
    {
        return;
    }

    dvec3 GetEmission()
    {
        return dvec3(0.0, 0.0, 0.0);
    }
};