#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

struct ImgLoader
{
    static GLuint LoadTexture(string &fileName);
};