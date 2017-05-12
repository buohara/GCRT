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

void RenderPass::Init(GLuint depthTexIn)
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fboWidth, fboHeight);    
    glUseProgram(renderProgram);

    // Depth map (on texture unit 2).

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    GLuint depthID = glGetUniformLocation(renderProgram, "depthTex");
    glUniform1i(depthID, 2);

    map<string, Model>::iterator it;

    for (it = models.begin(); it != models.end(); it++)
    {
        (*it).second.SetUniforms(cam, dirLights, ptLights, renderProgram);
        (*it).second.Draw();
    }
}