#pragma once

#include "GCRT.h"
#include "box.h"
#include "camera.h"
#include "plane.h"
#include "cylinder.h"
#include "sphere.h"
#include "model.h"
#include "shader.h"
#include "light.h"
#include "renderpass.h"

struct Scene
{
    Camera cam;
    bool wireFrame = false;
    bool useDOF = false;
    bool useBloom = false;

    map<string, Shader> shaders;
    map<string, Model> models;
    map<string, GLuint> textures;
    map<string, RMaterial> materials;

    vector<DirectionalLight> dirLights;
    vector<PointLight> ptLights;

    DepthPass depthPass;
    RenderPass renderPass;
    BloomPass bloomPass;
    DOFPass dofPass;

    GLuint dbFboID;

    void HandleInputs(MSG &msg);
    void Init();
    void LoadTextures();
    void InitMaterials();
    void InitModels();
    void InitLights();
    void Render(HDC hDC);

    void CreateNoiseTexture();
};