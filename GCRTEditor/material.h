#pragma once

#include "GCRT.h"
#include "imgloader.h"
#include "light.h"

using namespace std;
using namespace glm;

struct Tex
{
    GLuint texID;
    string imagePath;
};

struct RMaterial
{
    string name;
    vec3 kd;
    float spec;

    GLuint useNormalMap;
    GLuint useDiffuseMap;
    GLuint useSSS;
    GLuint useShadows;
    GLuint usePhong;
    GLuint useEnvMap;

    string diffTexName;
    string normalTexName;

    GLuint diffuseTexID;
    GLuint normalTexID;

    RMaterial();

    void ApplyMaterial(GLuint program);
    void SetNormalTex(GLuint normalTex, string name);
    void SetDiffuseTex(GLuint diffuseTex, string name);

    void UseSSS(bool bUse) { bUse ? useSSS = 1 : useShadows = 0; }
    void UseShadows(bool bUse) { bUse ? useShadows = 1 : useShadows = 0; }
    void UsePhong(bool bUse) { bUse ? usePhong = 1 : usePhong = 0; }
    void UseEnvMap(bool bUse) { bUse ? useEnvMap = 1 : useEnvMap = 0; }

    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights, GLuint program);
};