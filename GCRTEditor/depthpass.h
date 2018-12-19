#pragma once

#include "renderpass.h"

struct DepthPass : RenderPass
{
    GLuint dbFbo;
    GLuint depthProgram;
    GLuint depthTexOut;
    GLuint depthMapSize = 2048;

    void Init();
    void Render();
};