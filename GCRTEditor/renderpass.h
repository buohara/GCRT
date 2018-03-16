#pragma once

#include "GCRT.h"
#include "model.h"
#include "shader.h"
#include "scene.h"

struct RenderPass
{
    virtual void Render(Scene &scn) = 0;
};

/**
 * Main render pass. Draw every object in the scene using its particular
 * material properties.
 */

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
        GLuint depthTexIn, 
        uint32_t screenW,
        uint32_t screenH,
        bool useDOFIn,
        uint32_t msaaSamples
    );

    void CreateRenderFbo();
    void CreateMSAAFbo();
};

/**
 * Picker pass. Draw every object to a picker FBO using its unique picker color.
 * Use readPixel calls on this FBO and use the returned color to determine what
 * object was picked.
 */

struct PickerPass : RenderPass
{
    GLuint pickerFbo;
    GLuint pickerProgram;

    uint32_t fboWidth;
    uint32_t fboHeight;

    void InitOld(uint32_t screenW, uint32_t screenH);
    
    void GenFrameBuffers();
    void Resize(uint32_t w, uint32_t h);
    void Render(Scene &scn);
};

/**
 * Depth pass for shadow mapping and SSS.
 */

struct DepthPass : RenderPass
{
    GLuint dbFbo;
    GLuint depthProgram;
    GLuint depthTexOut;
    GLuint depthMapSize = 2048;

    void Init();
    void Render(Scene &scn);
    
    GLuint getDepthTex() { return depthTexID; }
    GLuint getDepthProgram() { return depthProgram; }
};

/**
 * Depth of field pass.
 */

struct DOFPass : RenderPass
{
    GLuint dofProgram;

    GLuint colorTexIn;
    GLuint noiseTexIn;
    GLuint renderFbo;

    GLuint vao;
    GLuint posVbo;
    GLuint uvVbo;

    uint32_t fboWidth;
    uint32_t fboHeight;

    float samplePts[64];

    void Init(
        GLuint colorTexIn, 
        GLuint noiseTexIn, 
        uint32_t screenW,
        uint32_t screenH
    );

    void Render(Scene &scn);
    void LoadQuadVerts();
    void GenerateSamplePoints();
};

/** 
 * A nice bloom and tone mapping pass.
 */

struct BloomPass : RenderPass
{
    GLuint brightProgram;
    GLuint blurProgram;
    GLuint composeProgram;

    GLuint colorTexIn;

    GLuint renderFboID;
    GLuint renderTexOut;

    uint32_t fboWidth;
    uint32_t fboHeight;

    GLuint brightTexOut;
    GLuint brightFboID;

    GLuint hBlurTexOut;
    GLuint hBlurFboID;

    GLuint vBlurTexOut;
    GLuint vBlurFboID;

    GLuint vaoID;
    GLuint posVboID;
    GLuint uvVboID;

    void GenFrameBuffers();
    void Resize(uint32_t w, uint32_t h);

    void Init(
        GLuint colorTexIn,
        uint32_t screenW,
        uint32_t screenH
    );

    void Render(Scene &scn);
    void LoadQuadVerts();
};