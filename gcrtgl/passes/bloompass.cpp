#include "bloompass.h"

extern RenderSettings g_settings;

/**
* [BloomPass::LoadQuadVerts description]
*/

void BloomPass::LoadQuadVerts()
{
    vector<vec3> pos;

    pos.push_back(vec3(-1.0, -1.0, 0.0));
    pos.push_back(vec3(1.0, 1.0, 0.0));
    pos.push_back(vec3(-1.0, 1.0, 0.0));
    pos.push_back(vec3(-1.0, -1.0, 0.0));
    pos.push_back(vec3(1.0, -1.0, 0.0));
    pos.push_back(vec3(1.0, 1.0, 0.0));

    vector<vec2> uvs;

    uvs.push_back(vec2(0.0, 0.0));
    uvs.push_back(vec2(1.0, 1.0));
    uvs.push_back(vec2(0.0, 1.0));
    uvs.push_back(vec2(0.0, 0.0));
    uvs.push_back(vec2(1.0, 0.0));
    uvs.push_back(vec2(1.0, 1.0));

    uint32_t numVerts = (uint32_t)pos.size();
    size_t posBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, posVbo);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &uvVbo);
    glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

/**
* [BloomPass::Init description]
* @param colorTexIn [description]
* @param renderFbo  [description]
* @param screenW    [description]
* @param screenH    [description]
*/

void BloomPass::Init(
    GLuint colorTexInput,
    bool renderToScreen
)
{
    // Compile bright, blur, and compose shaders.

    colorTexIn  = colorTexInput;

    Shader brightShader;
    Shader blurShader;
    Shader composeShader;

    brightShader.Create(
        string("BrightShader"),
        string("shaders/BrightShader.vert"),
        string("shaders/BrightShader.frag")
    );

    brightProgram = brightShader.program;

    blurShader.Create(
        string("GaussBlur"),
        string("shaders/GaussBlur.vert"),
        string("shaders/GaussBlur.frag")
    );

    blurProgram = blurShader.program;

    composeShader.Create(
        string("ComposeShader"),
        string("shaders/Compose.vert"),
        string("shaders/Compose.frag")
    );

    composeProgram = composeShader.program;
    LoadQuadVerts();

    // FBOs and textures for bright and blur passes.

    GenFrameBuffers();

    renderToScreen ? renderFbo = 0 : CreateRenderFbo();
}

/**
* [BloomPass::GenFrameBuffers description]
*/

void BloomPass::GenFrameBuffers()
{
    glGenFramebuffers(1, &brightFbo);

    glGenTextures(1, &brightTexOut);
    glBindTexture(GL_TEXTURE_2D, brightTexOut);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_settings.winW, g_settings.winH,
        0, GL_RGBA, GL_FLOAT, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, brightFbo);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        brightTexOut,
        0
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // HBLUR

    glGenFramebuffers(1, &hBlurFbo);

    glGenTextures(1, &hBlurTexOut);
    glBindTexture(GL_TEXTURE_2D, hBlurTexOut);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_settings.winW, g_settings.winH,
        0, GL_RGBA, GL_FLOAT, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, hBlurFbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hBlurTexOut, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // VBLUR

    glGenFramebuffers(1, &vBlurFbo);

    glGenTextures(1, &vBlurTexOut);
    glBindTexture(GL_TEXTURE_2D, vBlurTexOut);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_settings.winW, 
        g_settings.winH, 0, GL_RGBA, GL_FLOAT, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, vBlurFbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
        vBlurTexOut, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
* [BloomPass::Resize description]
* @param w [description]
* @param h [description]
*/

void BloomPass::Resize()
{
    GLuint framebuffers[3] = { brightFbo, hBlurFbo, vBlurFbo };
    GLuint fbTextures[3] = { brightTexOut, hBlurTexOut, vBlurTexOut };

    glDeleteFramebuffers(3, framebuffers);
    glDeleteTextures(3, fbTextures);

    GenFrameBuffers();
}

/**
* [BloomPass::Render description]
*/

void BloomPass::Render()
{
    // Bright pass.

    glBindFramebuffer(GL_FRAMEBUFFER, brightFbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, g_settings.winW, g_settings.winH);
    glUseProgram(brightProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexIn);
    GLuint colorID = glGetUniformLocation(brightProgram, "colorTex");
    glUniform1i(colorID, 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Horizontal and Vertical Blur passes.

    glBindFramebuffer(GL_FRAMEBUFFER, hBlurFbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, g_settings.winW, g_settings.winH);
    glUseProgram(blurProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brightTexOut);
    GLuint brightID = glGetUniformLocation(blurProgram, "colorTex");
    glUniform1i(brightID, 0);

    GLuint horizID = glGetUniformLocation(blurProgram, "horizontal");
    glUniform1i(horizID, 1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, vBlurFbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, g_settings.winW, g_settings.winH);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hBlurTexOut);
    GLuint hBlurID = glGetUniformLocation(blurProgram, "colorTex");
    glUniform1i(hBlurID, 0);

    horizID = glGetUniformLocation(blurProgram, "horizontal");
    glUniform1i(horizID, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Compose passes

    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, g_settings.winW, g_settings.winH);
    glUseProgram(composeProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexIn);
    GLuint composeColorTexID = glGetUniformLocation(composeProgram, "colorTex");
    glUniform1i(composeColorTexID, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vBlurTexOut);
    GLuint blurTexID = glGetUniformLocation(composeProgram, "blurTex");
    glUniform1i(blurTexID, 1);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

/**
 * BloomPass::CreateRenderFbo If not rendering to screen, create render FBO
 * resources.
 */

void BloomPass::CreateRenderFbo()
{
    glGenFramebuffers(1, &renderFbo);

    // Output texture.

    glGenTextures(1, &renderTexOut);
    glBindTexture(GL_TEXTURE_2D, renderTexOut);

    glTexImage2D(
        GL_TEXTURE_2D,
        0, GL_RGBA32F,
        g_settings.winW,
        g_settings.winH,
        0,
        GL_RGBA,
        GL_FLOAT,
        0)
        ;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach 

    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        renderTexOut,
        0
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}