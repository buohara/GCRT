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

    float samplePts[64];

    void Init(
        GLuint colorTexInput,
        GLuint noiseTexInput,
        bool renderToScreen
    );

    void Render();
    void LoadQuadVerts();
    void GenerateSamplePoints();
    void CreateRenderFbo();
};