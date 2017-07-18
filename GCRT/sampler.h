#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

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