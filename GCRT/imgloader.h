#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

struct Img
{
    uint32_t w;
    uint32_t h;
    vector<BYTE> pixels;
};

struct ImgLoader
{
    void LoadImg(string &fileName, Img &img);
};