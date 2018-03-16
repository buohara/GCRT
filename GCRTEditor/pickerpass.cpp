#include "pickerpass.h"

/**
 * [PickerPass::GenFrameBuffers description]
 */

void PickerPass::GenFrameBuffers()
{
    // Create a picker FBO with color and depth.

    glGenFramebuffers(1, &pickerFbo);

    GLuint pickerRenderBuffer;
    glGenRenderbuffers(1, &pickerRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, pickerRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, fboWidth, fboHeight);

    GLuint depthRenderBuffer;
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fboWidth, fboHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, pickerFbo);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER,
        pickerRenderBuffer
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
 * [PickerPass::Resize description]
 * @param w [description]
 * @param h [description]
 */

void PickerPass::Resize(uint32_t w, uint32_t h)
{
    glDeleteFramebuffers(1, &pickerFbo);
    fboWidth = w;
    fboHeight = h;
    GenFrameBuffers();
}

/**
 * [PickerPass::Init description]
 * @param screenW [description]
 * @param screenH [description]
 */

void PickerPass::Init(uint32_t screenW, uint32_t screenH)
{
    fboWidth = screenW;
    fboHeight = screenH;

    Shader pickerShader;
    pickerShader.Create(
        string("PickerPass"),
        string("PickerShaderAnim.vs"),
        string("PickerShaderAnim.fs")
    );

    pickerProgram = pickerShader.program;
    GenFrameBuffers();
}

/**
 * [PickerPass::Render description]
 * @param scn [description]
 */

void PickerPass::Render(Scene &scn)
{
    map<string, Model> models = scn.models;
    Camera cam = scn.cam;

    glBindFramebuffer(GL_FRAMEBUFFER, pickerFbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fboWidth, fboHeight);
    glUseProgram(pickerProgram);

    mat4 view = cam.GetView();
    GLuint viewID = glGetUniformLocation(pickerProgram, "view");
    glUniformMatrix4fv(viewID, 1, false, &view[0][0]);

    mat4 proj = cam.GetProjection();
    GLuint projID = glGetUniformLocation(pickerProgram, "proj");
    glUniformMatrix4fv(projID, 1, false, &proj[0][0]);

    /*map<string, Model>::iterator it;

    for (it = models.begin(); it != models.end(); it++)
    {
        vec3 pickerColor = (*it).second.pickerColor;
        GLuint pickerID = glGetUniformLocation(pickerProgram, "pickerColor");
        glUniform3fv(pickerID, 1, &pickerColor[0]);

        shared_ptr<Mesh> pMesh = scn.meshes[(*it).second.meshName];
        (*it).second.SetAnimMatrices(pickerProgram);
        pMesh->Draw();
    }*/

    for (auto &pair : models)
    {
        Model &model        = pair.second;
        vec3 pickerColor    = model.pickerColor;
        GLuint pickerID     = glGetUniformLocation(pickerProgram, "pickerColor");
        glUniform3fv(pickerID, 1, &pickerColor[0]);

        shared_ptr<Mesh> pMesh = scn.meshes[model.meshName];
        model.SetAnimMatrices(pickerProgram);
        pMesh->Draw();
    }
}