#pragma once

#include "gcrt.h"
#include "rtcamera.h"
#include "rtscene.h"
#include "sampler.h"
#include "surfaceintegrator.h"
#include "filter.h"
#include <iomanip>

const uint32_t maxWorkThreads = 15;

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
        imageW(512),
        imageH(512),
        vLightSets(16),
        vLightSetSize(16),
        camPathDepth(1),
        lightPathDepth(0),
        pixelSamples(4),
        filterSize(2),
        dofSamples(0),
        numThreads(8),
        xBlocks(4),
        yBlocks(4),
        numBSDFSamples(16),
        numLightSamples(8),
        camType(1),
        outputPath("../../gcrt/renders/"),
        frameFilePrefix("Frame")
    {}
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
    thread threads[maxWorkThreads];
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