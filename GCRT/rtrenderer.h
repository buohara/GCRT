#pragma once

#include "GCRT.h"
#include "rtscene.h"
#include "sampler.h"
#include "surfaceintegrator.h"

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
    //Filter *pFilter;

    vector<dvec3> *pImg;
    vector<Rect> *pImageBlocks;
};

DWORD WINAPI RenderThreadFunc(LPVOID lpParam);
long long GetMilliseconds();

struct RTRenderer
{
    uint32_t numThreads;
    ThreadData threadData[16];
    HANDLE hThreadArray[16];

    RTScene scn;
    vector<dvec3> image;
    vector<Rect> imageBlocks;
    Sampler sampler;
    SurfaceIntegrator integrator;

    uint32_t imageW;
    uint32_t imageH;

    void Render();

    void Init(uint32_t w, uint32_t h);
    void InitThreads();
    void SaveImage(string fileName);
};