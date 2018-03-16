#include "renderpass.h"

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

void DOFPass::InitOld(
    GLuint colorTexIn,
    GLuint noiseTexIn,
    GLuint renderFboIn,
    uint32_t screenW,
    uint32_t screenH
)
{
    colorTexID  = colorTexIn;
    noiseTexID  = noiseTexIn;
    renderFbo   = renderFboIn;
    fboHeight   = screenH;
    fboWidth    = screenW;

    LoadQuadVerts();
    GenerateSamplePoints();

    Shader dofShader;
    dofShader.Create(
        string("DOFShader"),
        string("DOFShader.vs"),
        string("DOFShader.fs")
    );

    dofProgram = dofShader.program;
}

/**
 * [DOFPass::Render description]
 */

void DOFPass::Render(Scene &scn, float t)
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
    glViewport(0, 0, fboWidth, fboHeight);
    glUseProgram(dofProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexID);
    GLuint colorID = glGetUniformLocation(dofProgram, "colorTex");
    glUniform1i(colorID, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noiseTexID);
    GLuint noiseID = glGetUniformLocation(dofProgram, "noiseTex");
    glUniform1i(noiseID, 1);

    GLuint sampleID = glGetUniformLocation(dofProgram, "samplePts");
    glUniform2fv(sampleID, 32, samplePts);

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}