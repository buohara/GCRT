#pragma once

#include "GCRT.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

using namespace std;
using namespace glm;

struct Texture
{
    string name;
    string imgPath;

    uint32_t w;
    uint32_t h;
    vector<dvec3> pixels;

    void Load(string imgPathIn);
    dvec3 Sample(dvec2 uv);
};