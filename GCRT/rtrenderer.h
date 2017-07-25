#pragma once

#include "GCRT.h"
#include "rtscene.h"
#include "sampler.h"
#include "surfaceintegrator.h"
#include "filter.h"

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
    uint32_t imageW;
    uint32_t imageH;

    RTScene *pScn;
    Sampler *pSampler;
    SurfaceIntegrator *pIntegrator;

    vector<vector<Sample>> *pImgSamples;
    vector<Rect> *pImageBlocks;
};

DWORD WINAPI RenderThreadFunc(LPVOID lpParam);
long long GetMilliseconds();

struct RTRenderSettings
{
    uint32_t imageW;
    uint32_t imageH;
    uint32_t sphereSamples;
    uint32_t pixelSamples;
    uint32_t filterSize;
    uint32_t dofSamples;
    uint32_t numThreads;
    uint32_t xBlocks;
    uint32_t yBlocks;
};

struct RTRenderer
{
    uint32_t numThreads;
    ThreadData threadData[16];
    HANDLE hThreadArray[16];

    RTScene scn;
    vector<vector<Sample>> imageSamples;
    vector<dvec3> outImage;

    vector<Rect> imageBlocks;

    Sampler sampler;
    SurfaceIntegrator integrator;
    Filter filter;

    uint32_t imageW;
    uint32_t imageH;

    RTRenderSettings settings;

    void Render();
    void FilterSamples();
    void GetFilterBox(int x, int y, int w, vector<uvec2> &filterBox);

    void Init();
    void InitThreads();
    void Preprocess();
    void GenerateVirtualLights();
    void SaveImage(string fileName);
    void LoadSettings(string file);
};