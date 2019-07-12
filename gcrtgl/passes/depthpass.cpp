#include "depthpass.h"

/**
 * [DepthPass::Init description]
 */

void DepthPass::Init()
{
    // Compile the depth pass shader.

    Shader depthShader;
    depthShader.Create(
        string("DepthPass"),
        string("shaders/DepthPassShaderAnim.vert"),
        string("shaders/DepthPassShaderAnim.frag")
    );

    // Create a depth FBO and associated texture.

    depthProgram = depthShader.program;

    glGenFramebuffers(1, &dbFbo);

    glGenTextures(1, &depthTexOut);
    glBindTexture(GL_TEXTURE_2D, depthTexOut);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT32,
        depthMapSize,
        depthMapSize,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        0
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, dbFbo);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depthTexOut,
        0
    );

    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * DepthPass::Render Loop over scene meshes and render depth.
 * @param scn Scene to render.
 */

void DepthPass::Render(Scene &scn)
{
    vector<Light> &lights = scn.lights;

    glBindFramebuffer(GL_FRAMEBUFFER, dbFbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, depthMapSize, depthMapSize);

    glUseProgram(depthProgram);

    vec3 lightPos   = lights[0].pos;
    vec3 lightLook  = lights[0].dir;

    mat4 depthView = lookAt(lightPos, lightLook, vec3(0.0, 1.0, 0.0));
    mat4 depthProj = ortho(-10.0, 10.0, -10.0, 10.0, 1.0, 100.0);

    GLuint viewID = glGetUniformLocation(depthProgram, "view");
    glUniformMatrix4fv(viewID, 1, false, &depthView[0][0]);

    GLuint projID = glGetUniformLocation(depthProgram, "proj");
    glUniformMatrix4fv(projID, 1, false, &depthProj[0][0]);

    for (auto &mesh : scn.meshes) mesh.second.Draw();
}