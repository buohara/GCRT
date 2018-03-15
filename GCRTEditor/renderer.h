#pragma once

#include "gcrt.h"
#include "scene.h"
#include "renderpass.h"
#include "imgui_impl_gcrt.h"
#include "input.h"

struct RenderSettings
{
    uint32_t winW;
    uint32_t winH;
    bool wireFrame;
    bool useDOF;
    bool useBloom;
    uint32_t msaaSamples;
    bool loadSceneFromFile;
    string sceneName;
};

struct Renderer
{
    RenderSettings settings;
    Scene scn;

    HWND hWnd;
    HDC hDC;

    PickerPass pickerPass;
    DepthPass depthPass;
    RenderPass renderPass;
    BloomPass bloomPass;
    DOFPass dofPass;

    GLuint dbFboID;
    GLuint renderFbo;
    GLuint renderTex;
    string selected;

    bool mouseDown[3];
    double mousePos[2];

    float t;

    static const uint32_t RENDER_TO_SCREEN = 0;
    
    vector<uint32_t> ImGuiKeys;

    void HandleInputs(MSG &msg);
    void Init();
    
    void CreateGLContext();
    void CreateRenderWindow(
        HINSTANCE hInstance,
        uint32_t width,
        uint32_t height,
        string title
    );

    void LoadDefaultScene();
    void LoadDefaultTextures();
    void InitDefaultMaterials();
    void InitDefaultModels();
    void InitDefaultLights();
    
    void Render();
    void DoPick(LPARAM mouseCoord);
    void UpdateImGui();
    void RenderSceneWindow();
    void RenderSelectionWindow();
    void RenderRendererWindow();
    void UpdateViewPorts(uint32_t w, uint32_t h);

    float nextPickClr[3];
    uint32_t nextPickIdx;
    vec3 nextPickerColor();

    void CreateNoiseTexture();
    void CreateRenderPassFbo();
    void ResizeRenderFbo();
};