#include "renderpass.h"

/**
 * [DepthPass::Init description]
 */

void DepthPass::Init()
{
    // Compile the depth pass shader.

    Shader depthShader;
    depthShader.Create(
        string("DepthPass"),
        string("DepthPassShaderAnim.vs"),
        string("DepthPassShaderAnim.fs")
    );

    // Create a depth FBO and associated texture.

    depthProgram = depthShader.program;

    glGenFramebuffers(1, &dbFboID);

    glGenTextures(1, &depthTexID);
    glBindTexture(GL_TEXTURE_2D, depthTexID);

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

    glBindFramebuffer(GL_FRAMEBUFFER, dbFboID);
    
    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depthTexID,
        0
    );
    
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * [DepthPass::Render description]
 * @param scn [description]
 */

void DepthPass::Render(Scene &scn)
{
    map<string, Model> &models = scn.models;
    vector<DirectionalLight> &dirLights = scn.dirLights;

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
        shared_ptr<Mesh> pMesh = scn.meshes[(*it).second.meshName];
        (*it).second.SetAnimMatrices(depthProgram);
        pMesh->Draw();
    }
}

/**
 * [PickerPass::GenFrameBuffers description]
 */

void PickerPass::GenFrameBuffers()
{
    // Create a picker FBO with color and depth.

    glGenFramebuffers(1, &pickerFboID);

    GLuint pickerRenderBuffer;
    glGenRenderbuffers(1, &pickerRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, pickerRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, fboWidth, fboHeight);

    GLuint depthRenderBuffer;
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fboWidth, fboHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, pickerFboID);
    
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
    glDeleteFramebuffers(1, &pickerFboID);
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

    glBindFramebuffer(GL_FRAMEBUFFER, pickerFboID);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fboWidth, fboHeight);
    glUseProgram(pickerProgram);

    mat4 view = cam.GetView();
    GLuint viewID = glGetUniformLocation(pickerProgram, "view");
    glUniformMatrix4fv(viewID, 1, false, &view[0][0]);

    mat4 proj = cam.GetProjection();
    GLuint projID = glGetUniformLocation(pickerProgram, "proj");
    glUniformMatrix4fv(projID, 1, false, &proj[0][0]);

    map<string, Model>::iterator it;

    for (it = models.begin(); it != models.end(); it++)
    {
        vec3 pickerColor = (*it).second.pickerColor;
        GLuint pickerID = glGetUniformLocation(pickerProgram, "pickerColor");
        glUniform3fv(pickerID, 1, &pickerColor[0]);

        shared_ptr<Mesh> pMesh = scn.meshes[(*it).second.meshName];
        (*it).second.SetAnimMatrices(pickerProgram);
        pMesh->Draw();
    }
}

/**
 * [RenderPass::Init description]
 * @param depthTexIn  [description]
 * @param renderFbo   [description]
 * @param screenW     [description]
 * @param screenH     [description]
 * @param useDOFIn    [description]
 * @param msaaSamples [description]
 */

void RenderPass::Init(
    GLuint depthTexIn, 
    GLuint renderFbo,
    uint32_t screenW,
    uint32_t screenH,
    bool useDOFIn,
    uint32_t msaaSamples
)
{
    Shader renderShader;
    wireFrame = false;
    useMSAA = false;

    if (msaaSamples > 1)
    {
        useMSAA = true;
    }

    renderShader.Create(
        string("RenderPass"),
        string("RenderShaderAnim.vs"),
        string("RenderShaderAnim.fs")
    );

    renderProgram = renderShader.program;
    depthTex = depthTexIn;

    fboWidth = screenW;
    fboHeight = screenH;

    renderFboID = renderFbo;
    useDOF = useDOFIn;

    if (useMSAA)
    {
        glGenTextures(1, &multisampleTexID);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleTexID);
        
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE,
            msaaSamples,
            GL_RGBA,
            fboWidth,
            fboHeight,
            true
        );

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        GLuint depthRenderBuffer;

        glGenRenderbuffers(1, &depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
        
        glRenderbufferStorageMultisample(
            GL_RENDERBUFFER, 
            msaaSamples, 
            GL_DEPTH_COMPONENT, 
            fboWidth, 
            fboHeight
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
}

/**
 * [RenderPass::Render description]
 * @param scn [description]
 */

void RenderPass::Render(Scene &scn, float t)
{
    map<string, Model> &models = scn.models;
    Camera cam = scn.cam;
    vector<DirectionalLight> dirLights = scn.dirLights;

    // Set wireframe.

    if (wireFrame)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Set MSAA FBO.

    if (useMSAA)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, multisampleFboID);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, renderFboID);
    }

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, fboWidth, fboHeight);
    glUseProgram(renderProgram);

    // Set depth map (on texture unit 2).

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    GLuint depthID = glGetUniformLocation(renderProgram, "depthTex");
    glUniform1i(depthID, 2);

    vec3 ka = vec3(0.3, 0.3, 0.3);
    GLuint kaID = glGetUniformLocation(renderProgram, "ka");
    glUniform3fv(kaID, 1, &ka[0]);

    // Set DOF parameters if enabled.

    if (useDOF == true)
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
        vec3 dirLightPos = dirLights[0].pos;
        vec3 dirLightLook = dirLights[0].look;

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

    map<string, Model>::iterator it;

    for (it = models.begin(); it != models.end(); it++)
    {
        RMaterial mat = scn.materials[(*it).second.matName];
        mat.ApplyMaterial(renderProgram);
        
        GLuint selectedID = glGetUniformLocation(renderProgram, "selected");
        glUniform1i(selectedID, (*it).second.selected);

        shared_ptr<Mesh> pMesh = scn.meshes[(*it).second.meshName];
        (*it).second.SetAnimMatrices(renderProgram);
        pMesh->Draw();
    }

    // Resolve MSAA buffer.

    if (useMSAA)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderFboID);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFboID);
        
        glBlitFramebuffer(
            0, 
            0, 
            fboWidth, 
            fboHeight, 
            0, 
            0, 
            fboWidth, 
            fboHeight, 
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 
            GL_NEAREST
        );
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/**
 * [DOFPass::LoadQuadVerts description]
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
 * GenerateSamplePoints
 */

void DOFPass::GenerateSamplePoints()
{
    float twoPi = 2.0f * glm::pi<float>();

    for (uint32_t i = 0; i < 32; i++)
    {
        float r = (float)rand() / (float)RAND_MAX;
        float theta = twoPi * ((float)rand() / (float)RAND_MAX);

        samplePts[2 * i] = r;
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
    GLuint colorTexIn,
    GLuint noiseTexIn,
    GLuint renderFboIn,
    uint32_t screenW,
    uint32_t screenH
)
{
    colorTexID = colorTexIn;
    noiseTexID = noiseTexIn;
    renderFbo = renderFboIn;
    fboHeight = screenH;
    fboWidth = screenW;

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

void DOFPass::Render()
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

void BloomPass::Init(
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

void BloomPass::Render()
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