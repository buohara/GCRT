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

    bool useDOF;
    GLuint depthTexIn;
    uint32_t fboWidth;
    uint32_t fboHeight;

    void Init(
        GLuint depthTexInput,
        uint32_t screenW,
        uint32_t screenH,
        bool useDOFIn,
        uint32_t msaaSamples,
        bool renderToScreen
    );

    void CreateRenderFbo();
    void CreateMSAAFbo(uint32_t msaaSamples);
};