#pragma once

#include "GCRT.h"
#include "imgloader.h"
#include "light.h"

using namespace std;
using namespace glm;

struct RMaterial
{
    string name;
    GLuint program;
    vec3 kd;

    GLuint useNormalMap;
    GLuint useDiffuseMap;
    GLuint useSSS;
    GLuint useShadows;

    GLuint depthTexID;
    GLuint diffuseTexID;
    GLuint normalTexID;
    GLuint selected;
    vec3 pickerColor;

    RMaterial();

    void ApplyMaterial(GLuint program);
    void SetDepthTex(GLuint depthTex);
    void SetNormalTex(GLuint normalTex);
    void SetDiffuseTex(GLuint diffuseTex);

    void UseSSS(bool bUse) { bUse ? useSSS = 1 : useShadows = 0; }
    void UseShadows(bool bUse) { bUse ? useShadows = 1 : useShadows = 0; }

    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights, GLuint program);
};