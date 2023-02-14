#pragma once

#include "gcrt.h"

using namespace std;
using namespace glm;

struct Sample
{
    dvec2 pos;
    dvec3 color;
};

struct Sampler
{
    uint32_t imageW;
    uint32_t imageH;
    uint32_t numSamples;

    void GenerateSamples(
        uint32_t x,
        uint32_t y,
        vector<dvec2> &samples
    );
};