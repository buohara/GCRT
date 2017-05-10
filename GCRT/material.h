#pragma once

#include "GCRT.h"
#include "imgloader.h"
#include "light.h"

using namespace std;
using namespace glm;

// Virtual base material class.

struct Material
{
    string name;
    GLuint program;

    virtual void ApplyMaterial() = 0;
    virtual void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights) = 0;
};

// Simple matte material with diffuse lighting, flat color.

struct BasicMaterial : Material
{
    vec3 kd;
    void ApplyMaterial();
    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights);
};

// Simple matte material with diffuse lighting, flat color, and shadow map.

struct BasicShadowMaterial : Material
{
    vec3 kd;
    GLuint depthTexID;
    void ApplyMaterial();
    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights);
};

// Material with diffuse lighting and normal map.

struct BumpMaterial : Material
{
    GLuint diffuseTexID;
    GLuint normalTexID;
    void ApplyMaterial();
    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights);
};

// Material with diffuse lighting, normal map, and shadow map.

struct BumpShadowMaterial : Material
{
    GLuint diffuseTexID;
    GLuint normalTexID;
    GLuint depthTexID;
    void ApplyMaterial();
    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights);
};

// Translucent material via SSS with matte color.

struct SSSMaterial : Material
{
    GLuint depthTexID;
    vec3 kd;
    void ApplyMaterial();
    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights);
};