#pragma once

#include "renderpass.h"

struct DOFPass : RenderPass
{
    GLuint dofProgram;

    GLuint colorTexIn;
    GLuint noiseTexIn;
    GLuint renderFbo;
    GLuint renderTexOut;

    GLuint vao;
    GLuint posVbo;
    GLuint uvVbo;

    uint32_t fboWidth;
    uint32_t fboHeight;

    float samplePts[64];

    void Init(
        GLuint colorTexInput,
        GLuint noiseTexInput,
        uint32_t screenW,
        uint32_t screenH,
        bool renderToScreen
    );

    void Render(Scene &scn);
    void LoadQuadVerts();
    void GenerateSamplePoints();
    void CreateRenderFbo()
};