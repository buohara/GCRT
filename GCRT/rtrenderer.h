#pragma once

#include "GCRT.h"
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

struct ThreadData
{
    uint32_t threadID;
};

DWORD WINAPI RenderThreadFunc(LPVOID lpParam);
long long GetMilliseconds();

struct RTRenderer
{
    uint32_t numThreads;
    ThreadData threadData[16];
    HANDLE hThreadArray[16];
    vector<dvec3> outImage;

    Sampler sampler;
    SurfaceIntegrator integrator;
    Filter filter;

    void Render();
    void FilterSamples();
    void GetFilterBox(int x, int y, int w, vector<uvec2> &filterBox);
    void ResetImageSamples();
    void GenerateImageBlocks();

    void Init();
    void InitThreads();
    void Preprocess();
    void GenerateVirtualLights();
    void SaveImage(string fileName);
    void LoadSettings(string file);
};