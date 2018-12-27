#pragma once

#include "gcrt.h"

using namespace std;
using namespace glm;

struct Noise
{
    map<uint32_t, vector<vec2>> gradients;
    void CreateGradients(uint32_t freq);
    float GetPerlin(uint32_t freq, float x, float y);
};