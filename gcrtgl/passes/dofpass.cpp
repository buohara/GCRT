#include "dofpass.h"

extern RenderSettings g_settings;

/**
 * DOFPass::LoadQuadVerts Load quad verts for DOF texture render.
 */

void DOFPass::LoadQuadVerts()
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

    uint32_t numVerts   = (uint32_t)pos.size();
    size_t posBufSize   = 3 * numVerts * sizeof(float);
    size_t uvBufSize    = 2 * numVerts * sizeof(float);

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
 * GenerateSamplePoints For each pixel, DOF samples depth of surround pixels
 * to compute focus. Generate Samples here.
 */

void DOFPass::GenerateSamplePoints()
{
    float twoPi = 2.0f * glm::pi<float>();

    for (uint32_t i = 0; i < 32; i++)
    {
        float r = (float)rand() / (float)RAND_MAX;
        float theta = twoPi * ((float)rand() / (float)RAND_MAX);

        samplePts[2 * i] = sqrt(r);
        samplePts[2 * i + 1] = theta;
    }
}

/**
 * [DOFPass::Init description]
 * @param colorTexIn  [description]
 * @param noiseTexIn  [description]
 * @param renderFboIn [description]
 * @param screenW     [description]
 * @param screenH     [description]
 */

void DOFPass::Init(
    GLuint colorTexInput,
    GLuint noiseTexInput,
    bool renderToScreen
)
{
    colorTexIn  = colorTexInput;
    noiseTexIn  = noiseTexInput;

    LoadQuadVerts();
    GenerateSamplePoints();

    Shader dofShader;
    dofShader.Create(
        string("DOFShader"),
        string("shaders/DOFShader.vert"),
        string("shaders/DOFShader.frag")
    );

    dofProgram = dofShader.program;

    if (renderToScreen)
        renderFbo = 0;
    else
        CreateRenderFbo();
}

/**
 * DOFPass::Render Process DOF.
 */

void DOFPass::Render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
    glViewport(0, 0, g_settings.winW, g_settings.winH);
    glUseProgram(dofProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexIn);
    GLuint colorID = glGetUniformLocation(dofProgram, "colorTex");
    glUniform1i(colorID, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noiseTexIn);
    GLuint noiseID = glGetUniformLocation(dofProgram, "noiseTex");
    glUniform1i(noiseID, 1);

    GLuint sampleID = glGetUniformLocation(dofProgram, "samplePts");
    glUniform2fv(sampleID, 32, samplePts);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

/**
 * DOFPass::CreateRenderFbo If not rendering to screen, create render FBO
 * resources.
 */

void DOFPass::CreateRenderFbo()
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