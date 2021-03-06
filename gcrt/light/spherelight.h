#pragma once

#include "gcrt.h"
#include "light.h"
#include "spheresampler.h"

using namespace std;
using namespace glm;

struct SphereLight : Light
{
    double r;
    dvec3 pos;
    SphereSampler sampler;

    void Intersect(Ray ray, Intersection &intsc);

    void Init(uint32_t sampleSets, uint32_t setSize);

    void GetLightSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut
    );

    double GetLightPDF(Ray rayIn, Intersection intsc);

    dvec3 EvalEmission(Ray rayOut, Intersection intsc);
};