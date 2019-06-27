#pragma once

#include "gcrt.h"
#include "rtcamera.h"
#include "rtscene.h"
#include "sampler.h"
#include "surfaceintegrator.h"
#include "filter.h"
#include <iomanip>

const uint32_t maxThreads = 16 - 1;

struct RTRenderSettings
{
    uint32_t imageW;
    uint32_t imageH;
    uint32_t vLightSets;
    uint32_t vLightSetSize;
    uint32_t camPathDepth;
    uint32_t lightPathDepth;
    uint32_t pixelSamples;
    uint32_t filterSize;
    uint32_t dofSamples;
    uint32_t numThreads;
    uint32_t xBlocks;
    uint32_t yBlocks;
    uint32_t numBSDFSamples;
    uint32_t numLightSamples;
    uint32_t camType;
    string outputPath;
    string frameFilePrefix;

    RTRenderSettings() :
        imageW(1920),
        imageH(1080),
        vLightSets(16),
        vLightSetSize(16),
        camPathDepth(2),
        lightPathDepth(0),
        pixelSamples(1),
        filterSize(2),
        dofSamples(0),
        xBlocks(8),
        yBlocks(8),
        numBSDFSamples(16),
        numLightSamples(8),
        camType(1),
        outputPath("../../gcrt/renders/"),
        frameFilePrefix("Frame")
    {
#ifdef _DEBUG
        numThreads = 1;
#else
        uint32_t concur = thread::hardware_concurrency();
        numThreads = std::max(1u, concur - 2);
        numThreads = std::min(numThreads, maxThreads);
#endif
    }
};

struct Rect
{
    uint32_t xmin;
    uint32_t xmax;
    uint32_t ymin;
    uint32_t ymax;
};

struct RTRenderer
{
    thread threads[maxThreads];
    vector<dvec3> outImage;

    Filter filter;

    RTRenderer() {};
    RTRenderer(RTRenderSettings settings) : settings(settings) {}

    RTRenderSettings settings;

    void Render(RTScene &scn);
    void FilterSamples();
    void GetFilterBox(int x, int y, int w, vector<uvec2> &filterBox);
    void ResetImageSamples();
    void GenerateImageBlocks();

    void Init();
    void Preprocess();
    void GenerateVirtualLights();
    void SaveImage(string fileName);
};