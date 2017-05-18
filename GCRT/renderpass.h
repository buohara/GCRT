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
    GLuint renderFboID;
    GLuint renderTex;
    bool lastPass;

    GLuint depthTex;
    uint32_t fboWidth;
    uint32_t fboHeight;

    void Init(GLuint depthTexIn, bool finalPass);
    GLuint getColorTex() { return renderTex; }
};

struct DepthPass
{
    GLuint dbFboID;
    GLuint depthProgram;
    GLuint depthTexID;
    GLuint depthMapSize = 2048;

    void Init();
    void Render(map<string, Model> &models, vector<DirectionalLight> &dirLights);
    
    GLuint getDepthTex() { return depthTexID; }
    GLuint getDepthProgram() { return depthProgram; }
};

struct DOFPass
{
    GLuint dofProgram;
    GLuint colorTexID;
    GLuint noiseTexID;

    GLuint vaoID;
    GLuint posVboID;
    GLuint uvVboID;

    float samplePts[64];

    void Init(GLuint colorTexIn, GLuint noiseTexIn);
    void Render();
    void LoadQuadVerts();
    void GenerateSamplePoints();
};

struct BloomPass
{
    GLuint brightProgram;
    GLuint blurProgram;
    GLuint composeProgram;

    GLuint colorTexID;
    
    GLuint brightTexID;
    GLuint brightFboID;

    GLuint hBlurTexID;
    GLuint hBlurFboID;

    GLuint vBlurTexID;
    GLuint vBlurFboID;

    GLuint vaoID;
    GLuint posVboID;
    GLuint uvVboID;

    void Init(GLuint colorTexIn);
    void Render();
    void LoadQuadVerts();
};