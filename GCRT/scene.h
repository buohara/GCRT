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
#include "imgui_impl_gcrt.h"

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

    PickerPass pickerPass;
    DepthPass depthPass;
    RenderPass renderPass;
    BloomPass bloomPass;
    DOFPass dofPass;

    GLuint dbFboID;
    GLuint renderFbo;
    GLuint renderTex;

    uint32_t winW;
    uint32_t winH;

    bool mouseDown[3];
    double mousePos[2];

    static const uint32_t RENDER_TO_SCREEN = 0;

    void HandleInputs(MSG &msg);
    void Init();
    void LoadTextures();
    void InitMaterials();
    void InitModels();
    void InitLights();
    void Render(HDC hDC);
    void DoPick(LPARAM mouseCoord);

    void CreateNoiseTexture();
    void CreateRenderPassFbo();
};