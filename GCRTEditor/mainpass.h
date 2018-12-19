#pragma once

#include "renderpass.h"

struct MainPass : RenderPass
{
    void Render();

    GLuint renderProgram;
    bool wireFrame;
    bool useMSAA;

    GLuint renderFbo;
    GLuint renderTexOut;

    GLuint multisampleFboID;
    GLuint multisampleTexID;
    GLuint multisampleDepthID;

    bool useDOF;
    GLuint depthTexIn;

    void Init(
        GLuint depthTexInput,
        bool useDOFIn,
        bool renderToScreen
    );

    void CreateRenderFbo();
    void CreateMSAAFbo();
};