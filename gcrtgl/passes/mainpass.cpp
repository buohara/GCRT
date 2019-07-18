#include "mainpass.h"

extern RenderSettings g_settings;

/**
 * MainPass::Init Initialize main render pass.
 *
 * @param depthTexIn  Input depth texture for shadow mapping.
 * @param screenW     Output FBO width.
 * @param screenH     Output FBO height.
 * @param useDOFIn    Set if using DOF, this pass needs to output focal distance data.
 * @param msaaSamples Number of MSAA samples to render.
 */

void MainPass::Init(
    GLuint depthTexInput,
    bool renderToScreen,
    Scene& scn
)
{
    Shader renderShader;
    wireFrame   = false;
    useMSAA     = g_settings.msaaSamples > 1;

    renderShader.Create(
        string("RenderPass"),
        string("shaders/RenderShaderAnim.vert"),
        string("shaders/RenderShaderAnim.frag")
    );

    renderProgram   = renderShader.program;
    depthTexIn      = depthTexInput;

    if (useMSAA) CreateMSAAFbo();

    if (g_settings.useSkyBox)
    {
        GLuint envMapID = glGetUniformLocation(renderProgram, "envMapTex");
        glUniform1i(envMapID, scn.skyTex.texID);
    }

    renderToScreen ? renderFbo = 0 : CreateRenderFbo();
}

/**
 * MainPass::CreateMSAAFbo If multisampling requested, create MSAA resources.
 *
 * @param msaaSamples Number of MSAA samples.
 */

void MainPass::CreateMSAAFbo()
{
    glGenTextures(1, &multisampleTexID);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleTexID);

    glTexImage2DMultisample(
        GL_TEXTURE_2D_MULTISAMPLE,
        g_settings.msaaSamples,
        GL_RGBA,
        g_settings.winW,
        g_settings.winH,
        true
    );

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    GLuint depthRenderBuffer;

    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);

    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER,
        g_settings.msaaSamples,
        GL_DEPTH_COMPONENT,
        g_settings.winW,
        g_settings.winH
    );

    glGenFramebuffers(1, &multisampleFboID);
    glBindFramebuffer(GL_FRAMEBUFFER, multisampleFboID);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D_MULTISAMPLE,
        multisampleTexID,
        0
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        depthRenderBuffer
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * MainPass::CreateRenderFbo If not rendering to screen, create render FBO
 * resources.
 */

void MainPass::CreateRenderFbo()
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

    // Depth attachment

    GLuint depthRenderBuffer;
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, g_settings.winW, g_settings.winH);

    // Attach 

    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        renderTexOut,
        0
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        depthRenderBuffer
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * [MainPass::Render description]
 * @param scn [description]
 */

void MainPass::Render(Scene &scn)
{
    Camera cam = scn.cam;
    vector<Light> &lights = scn.lights;

    glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
    glBindFramebuffer(GL_FRAMEBUFFER, useMSAA ? multisampleFboID : renderFbo);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, g_settings.winW, g_settings.winH);
    glUseProgram(renderProgram);

    // Set depth map (on texture unit 2).

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTexIn);
    GLuint depthID = glGetUniformLocation(renderProgram, "depthTex");
    glUniform1i(depthID, 2);

    vec3 ka = vec3(0.3, 0.3, 0.3);
    GLuint kaID = glGetUniformLocation(renderProgram, "ka");
    glUniform3fv(kaID, 1, &ka[0]);

    // Set DOF parameters if enabled.

    if (g_settings.useDOF == true)
    {
        GLuint useDOFID = glGetUniformLocation(renderProgram, "useDOF");
        glUniform1i(useDOFID, 1);

        vec4 planes = vec4(0.0, 8.0, 40.0, 120.0);
        GLuint focusPlanesID = glGetUniformLocation(renderProgram, "focusPlanes");
        glUniform4fv(focusPlanesID, 1, &planes[0]);
    }

    // Render models. 
    // 1. Camera uniforms

    {
        vec3 camPos = cam.pos;
        mat4 proj = cam.GetProjection();
        mat4 view = cam.GetView();

        GLuint viewID = glGetUniformLocation(renderProgram, "view");
        glUniformMatrix4fv(viewID, 1, false, &view[0][0]);

        GLuint projID = glGetUniformLocation(renderProgram, "proj");
        glUniformMatrix4fv(projID, 1, false, &proj[0][0]);

        GLuint camPosID = glGetUniformLocation(renderProgram, "camPos");
        glUniform3fv(camPosID, 1, &camPos[0]);
    }

    // 2. Lighting uniforms

    {
        vec3 dirLightPos    = lights[0].pos;
        vec3 dirLightLook   = lights[0].dir;

        mat4 depthView = lookAt(dirLightPos, dirLightLook, vec3(0.0, 1.0, 0.0));
        mat4 depthProj = ortho(-10.0, 10.0, -10.0, 10.0, 1.0, 100.0);

        GLuint lightPosID = glGetUniformLocation(renderProgram, "lightPos");
        glUniform3fv(lightPosID, 1, &dirLightPos[0]);

        GLuint lightViewID = glGetUniformLocation(renderProgram, "lightView");
        glUniformMatrix4fv(lightViewID, 1, false, &depthView[0][0]);

        GLuint lightProjID = glGetUniformLocation(renderProgram, "lightProj");
        glUniformMatrix4fv(lightProjID, 1, false, &depthProj[0][0]);
    }

    // 3. Material uniforms and draw.

    for (auto &mesh : scn.meshes)
    {
        RMaterial mat = scn.materials[mesh.second.matIdx];
        mat.ApplyMaterial(renderProgram);

        GLuint selectedID = glGetUniformLocation(renderProgram, "selected");
        glUniform1i(selectedID, mesh.second.selected);

        mesh.second.SetAnimMatrices(renderProgram);

        if (mesh.second.invert) glCullFace(GL_FRONT);
        mesh.second.Draw();
        if (mesh.second.invert) glCullFace(GL_BACK);
    }

    // Resolve MSAA buffer.

    if (useMSAA)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderFbo);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFboID);

        glBlitFramebuffer(
            0,
            0,
            g_settings.winW,
            g_settings.winH,
            0,
            0,
            g_settings.winW,
            g_settings.winH,
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
            GL_NEAREST
        );
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}