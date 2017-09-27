#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

struct CosHemiSampler
{
    vector<vector<dvec4>> samples;
    void GenerateSamples(uint32_t numSets, uint32_t setSize);
};