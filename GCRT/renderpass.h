#pragma once

#include "GCRT.h"
#include "model.h"
#include "shader.h"

// Main render pass. Draw every object in the scene using its particular
// material properties.

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

// Picker pass. Draw every object to a picker FBO using its unique picker color.
// Use readPixel calls on this FBO and use the returned color to determine what
// object was picked.

struct PickerPass
{
    GLuint pickerFboID;
    GLuint pickerProgram;

    uint32_t fboWidth;
    uint32_t fboHeight;

    void Init(uint32_t screenW, uint32_t screenH);
    void GenFrameBuffers();
    void Resize(uint32_t w, uint32_t h);
    void Render(map<string, Model> &models, Camera &cam);
};

// Depth pass for shadow mapping and SSS.

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

// Depth of field pass.

struct DOFPass
{
    GLuint dofProgram;

    GLuint colorTexID;
    GLuint noiseTexID;
    GLuint renderFbo;

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

// A nice bloom and tone mapping pass.

struct BloomPass
{
    GLuint brightProgram;
    GLuint blurProgram;
    GLuint composeProgram;

    GLuint colorTexID;

    GLuint renderFboID;
    uint32_t fboWidth;
    uint32_t fboHeight;

    GLuint brightTexID;
    GLuint brightFboID;

    GLuint hBlurTexID;
    GLuint hBlurFboID;

    GLuint vBlurTexID;
    GLuint vBlurFboID;

    GLuint vaoID;
    GLuint posVboID;
    GLuint uvVboID;

    void GenFrameBuffers();
    void Resize(uint32_t w, uint32_t h);

    void Init(
        GLuint colorTexIn,
        GLuint renderFbo,
        uint32_t screenW,
        uint32_t screenH
    );

    void Render();
    void LoadQuadVerts();
};