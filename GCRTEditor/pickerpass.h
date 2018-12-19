#pragma once

#include "renderpass.h"

struct PickerPass : RenderPass
{
    GLuint pickerFbo;
    GLuint pickerProgram;

    void Init();
    void GenFrameBuffers();
    void Resize();
    void Render();
};