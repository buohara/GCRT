#pragma once

#include "renderpass.h"

struct MainPass : RenderPass
{
    void Render(Scene &scn);

    GLuint renderProgram;
    bool wireFrame;
    bool useMSAA;

    GLuint renderFbo;
    GLuint renderTexOut;

    GLuint multisampleFboID;
    GLuint multisampleTexID;
    GLuint multisampleDepthID;

    GLuint depthTexIn;

    void Init(
        GLuint depthTexInput,
        bool renderToScreen,
        Scene &scn
    );

    void CreateRenderFbo();
    void CreateMSAAFbo();
};