#pragma once

#include "gcrtcommon.h"
#include "scene.h"
#include "renderpass.h"
#include "pickerpass.h"
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
    
    Renderer(HINSTANCE hInstance, string windowName);

    void HandleInputs(MSG &msg, Scene &scn);
    void Init(Scene &scn);
    
    void CreateGLContext();
    void CreateRenderWindow(
        HINSTANCE hInstance,
        string title
    );
    
    void Render(Scene &scn);
    void DoPick(LPARAM mouseCoord, Scene &scn);
    void UpdateViewPorts(Scene &scn);

    float nextPickClr[3];
    uint32_t nextPickIdx;
    vec3 nextPickerColor();

    void CreateNoiseTexture(Scene &scn);
    void ResizeRenderFbo();
};