#pragma once

#pragma once

#include "GCRT.h"
#include "scene.h"
#include "renderpass.h"
#include "imgui_impl_gcrt.h"

struct Selection
{
    string name;
    vec3 kd;
    float specular;
    vec3 scale;
    vec3 pos;
};

struct RenderSettings
{
    bool wireFrame;
    bool useDOF;
    bool useBloom;
};

struct Renderer
{
    RenderSettings settings;
    Scene scn;

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

    Selection selected;

    bool mouseDown[3];
    double mousePos[2];

    static const uint32_t RENDER_TO_SCREEN = 0;

    vector<uint32_t> ImGuiKeys;

    void HandleInputs(MSG &msg);
    void Init();
    void LoadTextures();
    void InitMaterials();
    void InitModels();
    void InitLights();
    void Render(HDC hDC);
    void DoPick(LPARAM mouseCoord);
    void UpdateImGui();
    void UpdateViewPorts(uint32_t w, uint32_t h);

    void CreateNoiseTexture();
    void CreateRenderPassFbo();
    void ResizeRenderFbo();
};