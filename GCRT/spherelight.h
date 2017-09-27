#pragma once

#include "GCRT.h"
#include "light.h"

using namespace std;
using namespace glm;

struct SphereLight : Light
{
    double r;
    dvec3 pos;

    virtual void GetLightSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut,
        vector<double> &weights
    );

    dvec3 GetEmission()
    {
    }
};