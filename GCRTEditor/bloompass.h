#pragma once

#include "renderpass.h"

struct BloomPass : RenderPass
{
    GLuint brightProgram;
    GLuint blurProgram;
    GLuint composeProgram;

    GLuint colorTexIn;

    GLuint renderFbo;
    GLuint renderTexOut;

    uint32_t fboWidth;
    uint32_t fboHeight;

    GLuint brightTexOut;
    GLuint brightFbo;

    GLuint hBlurTexOut;
    GLuint hBlurFbo;

    GLuint vBlurTexOut;
    GLuint vBlurFbo;

    GLuint vao;
    GLuint posVbo;
    GLuint uvVbo;

    void GenFrameBuffers();
    void CreateRenderFbo();
    void Resize(uint32_t w, uint32_t h);

    void Init(
        GLuint colorTexIn,
        uint32_t screenW,
        uint32_t screenH,
        bool renderToScreen
    );

    void Render(Scene &scn);
    void LoadQuadVerts();
};