#pragma once

#include "GCRT.h"
#include "raytrace.h"
#include "rtscene.h"

using namespace glm;

struct SurfaceIntegrator
{
    uint32_t numRays;
    
    dvec3 SampleSurface(
        Ray ray,
        RTScene &scn,
        Intersection intsc,
        uint32_t bounce,
        uint32_t maxBounces
    );
};