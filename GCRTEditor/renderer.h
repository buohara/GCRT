#pragma once

#include "gcrt.h"
#include "scene.h"
#include "renderpass.h"
#include "pickerpass.h"
#include "imgui_impl_gcrt.h"
#include "input.h"

struct Renderer
{
    HWND hWnd;
    HDC hDC;

    GLuint pickerFbo;

    map<string, shared_ptr<RenderPass>> passes;
    string selected;

    bool mouseDown[3];
    double mousePos[2];

    float t1;
    float t2;

    static const uint32_t RENDER_TO_SCREEN = 0;
    
    void HandleInputs(MSG &msg);
    void Init();
    
    void CreateGLContext();
    void CreateRenderWindow(
        HINSTANCE hInstance,
        string title
    );
    
    void Render();
    void DoPick(LPARAM mouseCoord);
    void UpdateViewPorts();

    float nextPickClr[3];
    uint32_t nextPickIdx;
    vec3 nextPickerColor();

    void CreateNoiseTexture();
    void ResizeRenderFbo();
};