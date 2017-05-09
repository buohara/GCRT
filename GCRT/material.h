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
    virtual void SetLights(vec3 lightPos) = 0;
};

struct BasicMaterial : Material
{
    vec3 kd;
    void ApplyMaterial();
    void SetLights(vec3 lightPos);
};

struct BasicShadowMaterial : Material
{
    vec3 kd;
    GLuint depthTexID;
    void ApplyMaterial();
    void SetLights(vec3 lightPos);
};

struct BumpMaterial : Material
{
    GLuint diffuseTexID;
    GLuint normalTexID;
    void ApplyMaterial();
    void SetLights(vec3 lightPos);
};