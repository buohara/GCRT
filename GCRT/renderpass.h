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

    // Render shader.

    GLuint renderProgram;

    // Output FBO.

    GLuint renderFboID;

    bool useDOF;
    GLuint depthTex;
    uint32_t fboWidth;
    uint32_t fboHeight;

    void Init(
        GLuint depthTexIn, 
        GLuint renderFbo,
        uint32_t screenW,
        uint32_t screenH,
        bool useDOFIn
    );
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
    // DOF shader.

    GLuint dofProgram;

    // Input image and per-pixel noise.

    GLuint colorTexID;
    GLuint noiseTexID;
    GLuint renderFbo;

    // Screen quad.

    GLuint vaoID;
    GLuint posVboID;
    GLuint uvVboID;

    uint32_t fboWidth;
    uint32_t fboHeight;

    float samplePts[64];

    void Init(
        GLuint colorTexIn, 
        GLuint noiseTexIn, 
        GLuint renderFboIn,
        uint32_t screenW,
        uint32_t screenH
    );

    void Render();
    void LoadQuadVerts();
    void GenerateSamplePoints();
};

struct BloomPass
{
    // Bright pass and blur shaders.

    GLuint brightProgram;
    GLuint blurProgram;
    GLuint composeProgram;

    // Input texture.

    GLuint colorTexID;
    
    // Output Fbo

    GLuint renderFboID;
    uint32_t fboWidth;
    uint32_t fboHeight;

    // Bright pass and blur FBOs/textures.

    GLuint brightTexID;
    GLuint brightFboID;

    GLuint hBlurTexID;
    GLuint hBlurFboID;

    GLuint vBlurTexID;
    GLuint vBlurFboID;

    // Render quad.

    GLuint vaoID;
    GLuint posVboID;
    GLuint uvVboID;

    void Init(
        GLuint colorTexIn,
        GLuint renderFbo,
        uint32_t screenW,
        uint32_t screenH
    );

    void Render();
    void LoadQuadVerts();
};