#pragma once

#include "GCRT.h"
#include "ray.h"

using namespace std;
using namespace glm;

struct Light
{
    dvec3 pwr;

    virtual void Intersect(Ray ray, Intersection &intsc) = 0;
    virtual void Init(uint32_t sampleSets, uint32_t setSize) = 0;

    virtual void GetLightSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut
    )
    {
        return;
    }

    virtual double GetLightPDF(Ray rayIn, Intersection intsc) = 0;

    virtual dvec3 EvalEmission(Ray rayOut, Intersection intsc)
    {
        return dvec3(0.0, 0.0, 0.0);
    }
};