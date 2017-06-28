#pragma once

#include "GCRT.h"
#include "rtscene.h"

using namespace glm;
using namespace std;

struct SurfaceIntegrator
{   
    vector<dvec4> sphereSamples;
    vector<dmat4> randomRotations;
    
    dmat4 NextRotation();

    void GenerateSamplePoints(uint32_t numSamples);

    dvec3 SampleSurface(
        Ray ray,
        RTScene &scn,
        Intersection intsc,
        uint32_t bounce,
        uint32_t maxBounces
    );
};