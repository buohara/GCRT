#pragma once

#include "GCRT.h"
#include "rtscene.h"

using namespace glm;
using namespace std;

struct SurfaceIntegrator
{   
    const double bias = 0.001;
    vector<dvec4> sphereSamples;
    vector<dmat4> randomRotations;
    
    uint32_t idx;
    uint32_t vLightSet;

    dmat4 NextRotation();
    uint32_t NextVLightSet();

    void GenerateSphereSamples(uint32_t numSamples);

    dvec3 SampleSurface(
        Ray rayIn,
        RTScene &scn,
        Intersection intsc,
        uint32_t bounce,
        uint32_t maxBounces
    );

    dvec3 CalcDiffuse(
        Ray rayIn,
        RTScene &scn,
        Intersection intsc,
        uint32_t bounce,
        uint32_t maxBounces
    );
};