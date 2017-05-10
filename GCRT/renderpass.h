#pragma once

#include "GCRT.h"
#include "model.h"
#include "shader.h"

struct RenderPass
{
    void Render(
        map<string, Model> &models, 
        vector<DirectionalLight> &dirLights,
        vector<PointLight> &ptLights,
        Camera &cam
    );

    uint32_t fboWidth;
    uint32_t fboHeight;

    void Init();
};

struct DepthPass
{
    GLuint dbFboID;
    GLuint depthProgram;
    GLuint depthTexID;
    GLuint depthMapSize = 1024;

    void Init();
    void Render(map<string, Model> &models, vector<DirectionalLight> &dirLights);
    
    GLuint getDepthTex() { return depthTexID; }
    GLuint getDepthProgram() { return depthProgram; }
};