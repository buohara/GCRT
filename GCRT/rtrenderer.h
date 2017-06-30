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
    Rect rect;
    uint32_t imageW;
    uint32_t imageH;

    RTScene *pScn;
    vector<dvec3> *pImg;
};

DWORD WINAPI RenderThreadFunc(LPVOID lpParam);

struct RTRenderer
{
    uint32_t numThreads;
    ThreadData threadData[16];
    HANDLE hThreadArray[16];

    RTScene scn;
    vector<dvec3> image;

    uint32_t imageW;
    uint32_t imageH;

    void Render();

    void Init(uint32_t w, uint32_t h);
    void InitThreads();
    void SaveImage(string fileName);
};