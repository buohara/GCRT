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
};

struct PathDebugData
{
    dvec3 rayOrigins[16];
    uint32_t intscMaterials[16];
    uint32_t stackPtr;
};

struct SurfaceIntegrator
{   
    const double bias = 0.01;
    
    uint32_t idx;
    uint32_t curVLightSet;
    uint32_t numVLightSets;
    uint32_t vLightSetSize;
    uint32_t numLightSamples;

    void NextVLightSet();

    void SampleBSDF(
        vector<SurfSample> &surfSamples,
        uint32_t bounce,
        uint32_t maxBounces,
        Ray rayIn,
        RTScene &scn,
        Intersection intsc
    );
    
    void SampleLightDistribution(
        vector<SurfSample> &surfSamples,
        uint32_t bounce,
        uint32_t maxBounces,
        Ray rayIn,
        RTScene &scn,
        Intersection intsc
    );

    dvec3 SampleSurface(
        Ray rayIn,
        RTScene &scn,
        Intersection intsc,
        uint32_t bounce,
        uint32_t maxBounces
    );

    dvec3 ApplyBalanceHeuristic(
        vector<SurfSample> &bsdfSamples,
        vector<SurfSample> &lightSamples
    );
    
    dvec3 SampleVirtualLights(
        Ray rayIn,
        RTScene &scn,
        Intersection intsc,
        uint32_t bounce,
        uint32_t maxBounces
    );

    void DumpPath();
};