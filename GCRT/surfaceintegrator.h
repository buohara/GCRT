#pragma once

#include "GCRT.h"
#include "rtscene.h"

using namespace glm;
using namespace std;

enum DistType
{
    BSDF_TYPE,
    LIGHT_TYPE,
    MAX_TYPES
};

struct SurfSample
{
    dvec3 BSDF;
    double BSDFPDF;
    double LightPDF;
    DistType distType;
};

struct SurfaceIntegrator
{   
    const double bias = 0.002;
    
    uint32_t idx;
    uint32_t curVLightSet;
    uint32_t numVLightSets;
    uint32_t vLightSetSize;
    uint32_t numLightSamples;

    void NextVLightSet();

    uint32_t SampleBSDF(
        vector<SurfSample> &surfSamples,
        uint32_t bounce,
        uint32_t maxBounces,
        Ray rayIn,
        RTScene &scn,
        Intersection intsc,
        uint32_t &totalSamples
    );
    
    uint32_t SampleLightDistribution(
        vector<SurfSample> &surfSamples,
        uint32_t bounce,
        uint32_t maxBounces,
        Ray rayIn,
        RTScene &scn,
        Intersection intsc,
        uint32_t &totalSamples
    );

    dvec3 SampleSurface(
        Ray rayIn,
        RTScene &scn,
        Intersection intsc,
        uint32_t bounce,
        uint32_t maxBounces
    );

    dvec3 ApplyBalanceHeuristic(
        vector<SurfSample> &surfSamples,
        uint32_t nBSDFSamples,
        uint32_t nLightSamples
    );
    
    dvec3 SampleVirtualLights(
        Ray rayIn,
        RTScene &scn,
        Intersection intsc,
        uint32_t bounce,
        uint32_t maxBounces
    );
};