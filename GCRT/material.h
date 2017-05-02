#pragma once

#include "GCRT.h"
#include "imgloader.h"

using namespace std;
using namespace glm;

struct Material
{
    string name;
    GLuint textureID;
    vec3 kd;
    vec3 ka;

    void LoadTexture(string &fileName);
    void SetDiffuse(vec3 kd);
    void SetAmbient(vec3 ka);
    void SetShaderParams(GLuint progID);
};