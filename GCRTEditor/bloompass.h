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
    void Resize();

    void Init(
        GLuint colorTexIn,
        bool renderToScreen
    );

    void Render();
    void LoadQuadVerts();
};