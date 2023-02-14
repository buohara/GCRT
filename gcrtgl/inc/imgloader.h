#pragma once

#include "gcrtgl.h"

using namespace std;
using namespace glm;

struct ImgLoader
{
    static GLuint LoadTexture(string fileName);
};