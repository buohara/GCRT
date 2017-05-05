#pragma once

#include "GCRT.h"
#include "imgloader.h"

using namespace std;
using namespace glm;

struct Material
{
    string name;
    GLuint program;
    virtual void ApplyMaterial() = 0;
};

struct BumpMaterial : Material
{
    GLuint diffuseTexID;
    GLuint normalTexID;
    void ApplyMaterial();
};