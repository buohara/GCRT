#include "renderpass.h"

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

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, uvVboID);
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

void BloomPass::InitOld(
    GLuint colorTexIn,
    GLuint renderFbo,
    uint32_t screenW,
    uint32_t screenH
)
{
    // Compile bright, blur, and compose shaders.

    colorTexID = colorTexIn;
    renderFboID = renderFbo;
    fboWidth = screenW;
    fboHeight = screenH;

    Shader brightShader;
    Shader blurShader;
    Shader composeShader;

    brightShader.Create(
        string("BrightShader"),
        string("BrightShader.vs"),
        string("BrightShader.fs")
    );

    brightProgram = brightShader.program;

    blurShader.Create(
        string("GaussBlur"),
        string("GaussBlur.vs"),
        string("GaussBlur.fs")
    );

    blurProgram = blurShader.program;

    composeShader.Create(
        string("ComposeShader"),
        string("Compose.vs"),
        string("Compose.fs")
    );

    composeProgram = composeShader.program;
    LoadQuadVerts();

    // FBOs and textures for bright and blur passes.

    GenFrameBuffers();
}

/**
* [BloomPass::GenFrameBuffers description]
*/

void BloomPass::GenFrameBuffers()
{
    glGenFramebuffers(1, &brightFboID);

    glGenTextures(1, &brightTexID);
    glBindTexture(GL_TEXTURE_2D, brightTexID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fboWidth, fboHeight, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, brightFboID);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        brightTexID,
        0
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // HBLUR

    glGenFramebuffers(1, &hBlurFboID);

    glGenTextures(1, &hBlurTexID);
    glBindTexture(GL_TEXTURE_2D, hBlurTexID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fboWidth, fboHeight, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, hBlurFboID);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hBlurTexID, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // VBLUR

    glGenFramebuffers(1, &vBlurFboID);

    glGenTextures(1, &vBlurTexID);
    glBindTexture(GL_TEXTURE_2D, vBlurTexID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fboWidth, fboHeight, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, vBlurFboID);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vBlurTexID, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
* [BloomPass::Resize description]
* @param w [description]
* @param h [description]
*/

void BloomPass::Resize(uint32_t w, uint32_t h)
{
    GLuint framebuffers[3] = { brightFboID, hBlurFboID, vBlurFboID };
    GLuint fbTextures[3] = { brightTexID, hBlurTexID, vBlurTexID };

    fboWidth = w;
    fboHeight = h;

    glDeleteFramebuffers(3, framebuffers);
    glDeleteTextures(3, fbTextures);

    GenFrameBuffers();
}

/**
* [BloomPass::Render description]
*/

void BloomPass::Render(Scene &scn, float t)
{
    // Do bright pass.

    glBindFramebuffer(GL_FRAMEBUFFER, brightFboID);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fboWidth, fboHeight);
    glUseProgram(brightProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexID);
    GLuint colorID = glGetUniformLocation(brightProgram, "colorTex");
    glUniform1i(colorID, 0);

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Do Horizontal and Vertical Blur passes.

    glBindFramebuffer(GL_FRAMEBUFFER, hBlurFboID);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fboWidth, fboHeight);
    glUseProgram(blurProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brightTexID);
    GLuint brightID = glGetUniformLocation(blurProgram, "colorTex");
    glUniform1i(brightID, 0);

    GLuint horizID = glGetUniformLocation(blurProgram, "horizontal");
    glUniform1i(horizID, 1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, vBlurFboID);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fboWidth, fboHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hBlurTexID);
    GLuint hBlurID = glGetUniformLocation(blurProgram, "colorTex");
    glUniform1i(hBlurID, 0);

    horizID = glGetUniformLocation(blurProgram, "horizontal");
    glUniform1i(horizID, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Compose final image

    glBindFramebuffer(GL_FRAMEBUFFER, renderFboID);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fboWidth, fboHeight);
    glUseProgram(composeProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexID);
    GLuint composeColorTexID = glGetUniformLocation(composeProgram, "colorTex");
    glUniform1i(composeColorTexID, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vBlurTexID);
    GLuint blurTexID = glGetUniformLocation(composeProgram, "blurTex");
    glUniform1i(blurTexID, 1);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}