#pragma once

#include "gcrtgl.h"

using namespace std;

struct Shader
{
    string name;
    GLuint program;

    void Create(string &nameIn, string &vsFileName, string &fsFileName);
};