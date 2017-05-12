#pragma once

#include "GCRT.h"
#include "model.h"
#include "shader.h"

struct RenderPass
{
    void Render(
        map<string, Model> &models,
        Camera &cam,
        vector<DirectionalLight> &dirLights,
        vector<PointLight> &ptLights
    );

    GLuint renderProgram;
    GLuint depthTex;
    uint32_t fboWidth;
    uint32_t fboHeight;

    void Init(GLuint depthTexIn);
};

struct DepthPass
{
    GLuint dbFboID;
    GLuint depthProgram;
    GLuint depthTexID;
    GLuint depthMapSize = 4096;

    void Init();
    void Render(map<string, Model> &models, vector<DirectionalLight> &dirLights);
    
    GLuint getDepthTex() { return depthTexID; }
    GLuint getDepthProgram() { return depthProgram; }
};