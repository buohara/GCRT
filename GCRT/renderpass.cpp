#include "renderpass.h"

/**
 * Init - Initialize depth FBO and associated textures.
 */

void DepthPass::Init()
{
    // Compile the depth pass shader.

    Shader depthShader;
    depthShader.Create(
        string("DepthPass"),
        string("DepthPassShader.vs"),
        string("DepthPassShader.fs")
    );

    // Create a depth FBO and associated texture.

    depthProgram = depthShader.program;

    glGenFramebuffers(1, &dbFboID);

    glGenTextures(1, &depthTexID);
    glBindTexture(GL_TEXTURE_2D, depthTexID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, depthMapSize, depthMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, dbFboID);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexID, 0);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * Render - Render the scene from each directional light's perspective into the depth
 * map.
 */

void DepthPass::Render(map<string, Model> &models, vector<DirectionalLight> &dirLights)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dbFboID);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, depthMapSize, depthMapSize);

    glUseProgram(depthProgram);

    vec3 lightPos = dirLights[0].pos;
    vec3 lightLook = dirLights[0].look;

    mat4 depthView = lookAt(lightPos, lightLook, vec3(0.0, 1.0, 0.0));
    mat4 depthProj = ortho(-10.0, 10.0, -10.0, 10.0, 1.0, 100.0);

    GLuint viewID = glGetUniformLocation(depthProgram, "view");
    glUniformMatrix4fv(viewID, 1, false, &depthView[0][0]);

    GLuint projID = glGetUniformLocation(depthProgram, "proj");
    glUniformMatrix4fv(projID, 1, false, &depthProj[0][0]);

    map<string, Model>::iterator it;

    for (it = models.begin(); it != models.end(); it++)
    {
        mat4 model = (*it).second.pGeom->model;

        GLuint modelID = glGetUniformLocation(depthProgram, "model");
        glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

        (*it).second.pGeom->Draw();
    }
}

/**
 * Initialize the render pass.
 */

void RenderPass::Init(GLuint depthTexIn, bool finalPass)
{
    Shader renderShader;
    
    renderShader.Create(
        string("RenderPass"),
        string("RenderShader.vs"),
        string("RenderShader.fs")
    );

    renderProgram = renderShader.program;
    depthTex = depthTexIn;

    fboWidth = 1920;
    fboHeight = 1080;

    lastPass = finalPass;

    if (finalPass == false)
    {
        glGenFramebuffers(1, &renderFboID);
        
        glGenTextures(1, &renderTex);
        glBindTexture(GL_TEXTURE_2D, renderTex);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2560, 1440, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        GLuint depthrenderbuffer;
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2560, 1440);

        glBindFramebuffer(GL_FRAMEBUFFER, renderFboID);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

/**
 * Render - Main render pass. Draw every object in the scene using it's particular material.
 */

void RenderPass::Render(
    map<string, Model> &models,
    Camera &cam,
    vector<DirectionalLight> &dirLights,
    vector<PointLight> &ptLights
)
{
    if (lastPass == true)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, renderFboID);
    }

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fboWidth, fboHeight);    
    glUseProgram(renderProgram);

    // Depth map (on texture unit 2).

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    GLuint depthID = glGetUniformLocation(renderProgram, "depthTex");
    glUniform1i(depthID, 2);

    vec3 ka = vec3(0.3, 0.3, 0.3);
    GLuint kaID = glGetUniformLocation(renderProgram, "ka");
    glUniform3fv(kaID, 1, &ka[0]);

    if (lastPass == false)
    {
        GLuint useDOFID = glGetUniformLocation(renderProgram, "useDOF");
        glUniform1i(useDOFID, 1);

        vec4 planes = vec4(4.0, 5.0, 20.0, 40.0);
        GLuint focusPlanesID = glGetUniformLocation(renderProgram, "focusPlanes");
        glUniform4fv(focusPlanesID, 1, &planes[0]);
    }

    map<string, Model>::iterator it;

    for (it = models.begin(); it != models.end(); it++)
    {
        (*it).second.SetUniforms(cam, dirLights, ptLights, renderProgram);
        (*it).second.Draw();
    }
}

/**
 * LoadQuadVerts
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

    uint32_t numVerts = pos.size();
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
 * GenerateSamplePoints
 */

void DOFPass::GenerateSamplePoints()
{
    float twoPi = 2.0 * glm::pi<float>();
    float r;
    float theta;

    for (uint32_t i = 0; i < 16; i++)
    {
        r = (float)rand() / (float)RAND_MAX;
        theta = twoPi * ((float)rand() / (float)RAND_MAX);

        samplePts[2 * i] = r * cosf(theta);
        samplePts[2 * i + 1] = r * sinf(theta);
    }
}

/**
 * Init
 */

void DOFPass::Init(GLuint colorTexIn)
{
    colorTexID = colorTexIn;
    
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
* Init
*/

void DOFPass::Render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 2560, 1440);
    glUseProgram(dofProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexID);
    GLuint colorID = glGetUniformLocation(dofProgram, "colorTex");
    glUniform1i(colorID, 0);

   GLuint sampleID = glGetUniformLocation(dofProgram, "samplePts");
   glUniform2fv(sampleID, 16, samplePts);

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}