#pragma once

#include "renderpass.h"

struct PickerPass : RenderPass
{
    GLuint pickerFbo;
    GLuint pickerProgram;

    uint32_t fboWidth;
    uint32_t fboHeight;

    void Init(uint32_t screenW, uint32_t screenH);

    void GenFrameBuffers();
    void Resize(uint32_t w, uint32_t h);
    void Render(Scene &scn);
};