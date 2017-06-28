#pragma once

#include "GCRT.h"
#include "rtscene.h"
#include "sampler.h"
#include "surfaceintegrator.h"

struct RTRenderer
{
    RTScene scn;
    Sampler sampler;
    SurfaceIntegrator integrator;

    vector<dvec3> image;

    uint32_t imageW;
    uint32_t imageH;

    void Render();
    void Init(uint32_t w, uint32_t h);
    void InitScene();
    void SaveImage(string fileName);
};