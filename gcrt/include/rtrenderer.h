#pragma once

#include "gcrt.h"
#include "rtcamera.h"
#include "rtscene.h"
#include "sampler.h"
#include "surfaceintegrator.h"
#include "filter.h"
#include <iomanip>

struct Rect
{
    uint32_t xmin;
    uint32_t xmax;
    uint32_t ymin;
    uint32_t ymax;
};

struct RTRenderer
{
    uint32_t numThreads;
    thread threads[16];
    vector<dvec3> outImage;

    SurfaceIntegrator integrator;
    Filter filter;

    void Render();
    void FilterSamples();
    void GetFilterBox(int x, int y, int w, vector<uvec2> &filterBox);
    void ResetImageSamples();
    void GenerateImageBlocks();

    void Init();
    void Preprocess();
    void GenerateVirtualLights();
    void SaveImage(string fileName);
};