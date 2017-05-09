#include "scene.h"

/**
 * Init - Initialize scene assets.
 */

void Scene::Init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
    glClearDepth(1.0f);

    cam.Init(
        vec3(15.0, 15.0, 15.0),
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        4.0f / 3.0f,
        45.0f,
        0.1f,
        100.f
    );
    
    // DEVELOPMENT - Shadow mapping/SSS. Generate a depth FBO to draw into and associated
    // texture to read from.

    glGenFramebuffers(1, &dbFboID);

    GLuint depthTexID = 0;
    glGenTextures(1, &depthTexID);
    glBindTexture(GL_TEXTURE_2D, depthTexID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    textures["DepthTex"] = depthTexID;

    glBindFramebuffer(GL_FRAMEBUFFER, dbFboID);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexID, 0);
    glDrawBuffer(GL_NONE);

    // DEVELOPMENT

    LoadShaders();
    LoadTextures();
    InitMaterials();
    InitModels();
}

/**
 * LoadShaders - Compile shaders and cache their handles for later use.
 */

void Scene::LoadShaders()
{
    shaders["Basic"].Create(
        string("BasicShader"),
        string("BasicShader.vs"),
        string("BasicShader.fs")
    );

    shaders["DepthPass"].Create(
        string("DepthPass"),
        string("DepthPassShader.vs"),
        string("DepthPassShader.fs")
    );

    shaders["BasicShadow"].Create(
        string("BasicShadow"),
        string("BasicShadow.vs"),
        string("BasicShadow.fs")
    );

    shaders["Bump"].Create(
        string("Bump"),
        string("BumpShader.vs"),
        string("BumpShader.fs")
    );
}

/**
 * LoadTextures - Read texture images from file and create GL textures.
 */

void Scene::LoadTextures()
{
    textures["DirtDiffuse"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/dirtdiffuse.jpg"));

    textures["DirtNormal"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/dirtnormal.jpg"));

    textures["GrassDiffuse"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/grassdiffuse.jpg"));

    textures["GrassNormal"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/grassNormal.jpg"));
}

/**
 * InitMaterials - Build materials from shaders and textures.
 */

void Scene::InitMaterials()
{
    BumpMaterial dirtMat;
    dirtMat.name = "Dirt";
    dirtMat.diffuseTexID = textures["DirtDiffuse"];
    dirtMat.normalTexID = textures["DirtNormal"];
    dirtMat.program = shaders["Bump"].program;
    materials["Dirt"] = make_shared<BumpMaterial>(dirtMat);

    BumpMaterial grassMat;
    dirtMat.name = "Grass";
    dirtMat.diffuseTexID = textures["GrassDiffuse"];
    dirtMat.normalTexID = textures["GrassNormal"];
    dirtMat.program = shaders["Bump"].program;
    materials["Grass"] = make_shared<BumpMaterial>(dirtMat);

    BasicMaterial basicBlueMat;
    basicBlueMat.name = "BasicBlue";
    basicBlueMat.program = shaders["Basic"].program;
    basicBlueMat.kd = vec3(0.1, 0.1, 0.7);
    materials["BasicBlue"] = make_shared<BasicMaterial>(basicBlueMat);

    BasicShadowMaterial basicShadowMat;
    basicShadowMat.name = "BasicShadow";
    basicShadowMat.program = shaders["BasicShadow"].program;
    basicShadowMat.kd = vec3(0.1, 0.1, 0.7);
    basicShadowMat.depthTexID = textures["DepthTex"];
    materials["BasicShadow"] = make_shared<BasicShadowMaterial>(basicShadowMat);
}

/**
 * Create models from geometries and materials.
 */

void Scene::InitModels()
{
    // Plane

    Plane pln;
    pln.Create(10, 10);
    pln.Scale(vec3(20.0, 20.0, 1.0));

    models["Plane"].pGeom = make_shared<Plane>(pln);
    models["Plane"].SetMaterial(materials["BasicShadow"]);

    //Box box;
    //box.Create();
    //box.Scale(vec3(1.0, 1.0, 1.0));
    //box.Translate(vec3(-5.0, 5.0, 1.0));

    //scn.models["Box"].pGeom = make_shared<Box>(box);
    //scn.models["Box"].SetMaterial(scn.materials["Dirt"]);

    Sphere sph;
    sph.Create(25, 25);
    sph.Scale(vec3(5.0, 5.0, 5.0));
    sph.Translate(vec3(0.0, 0.0, 3.0));

    models["Sphere"].pGeom = make_shared<Sphere>(sph);
    models["Sphere"].SetMaterial(materials["BasicShadow"]);

    Cylinder cyl;
    cyl.Create(20);
    cyl.Scale(vec3(2.0, 2.0, 2.0));
    cyl.Translate(vec3(-5.0, -5.0, 3.0));

    //models["Cylinder"].pGeom = make_shared<Cylinder>(cyl);
    //models["Cylinder"].SetMaterial(materials["BasicBlue"]);
}

/**
 * Render - Loop through each model in the scene and draw it.
 */

void Scene::Render(HDC hDC)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float t = 0;

    // Update camera and get projection/view matrices.

    cam.Update();

    // DEVELOPMENT - Depth render pass

    map<string, Model>::iterator it;
    GLuint depthProgram = shaders["DepthPass"].program;

    glUseProgram(depthProgram);
    
    vec3 lightPos(15.0, -15.0, 15.0);
    mat4 depthView = lookAt(lightPos, vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
    mat4 depthProj = ortho(-30.0, 30.0, -30.0, 30.0, 0.1, 100.0);

    glBindFramebuffer(GL_FRAMEBUFFER, dbFboID);
    glViewport(0, 0, 1024, 1024);

    for (it = models.begin(); it != models.end(); it++)
    {
        mat4 model = (*it).second.pGeom->model;

        GLuint modelID = glGetUniformLocation(depthProgram, "model");
        glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

        GLuint viewID = glGetUniformLocation(depthProgram, "view");
        glUniformMatrix4fv(viewID, 1, false, &depthView[0][0]);

        GLuint projID = glGetUniformLocation(depthProgram, "proj");
        glUniformMatrix4fv(projID, 1, false, &depthProj[0][0]);

        (*it).second.pGeom->Draw();
    }

    // Camera render pass.

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1920, 1080);

    for (it = models.begin(); it != models.end(); it++)
    {
        GLuint modelProgram = (*it).second.program;

        glUseProgram(modelProgram);

        GLuint lightViewID = glGetUniformLocation(modelProgram, "lightView");
        glUniformMatrix4fv(lightViewID, 1, false, &depthView[0][0]);

        GLuint lightProjID = glGetUniformLocation(modelProgram, "lightProj");
        glUniformMatrix4fv(lightProjID, 1, false, &depthProj[0][0]);

        (*it).second.SetCamera(cam);
        (*it).second.SetLights(lightPos);
        (*it).second.Draw();
    }

    SwapBuffers(hDC);
    t += 0.01f;
}

/**
 * HandleInputs - Process any keyboard and mouse inputs.
 *
 * @param msg Windows event message to handle.
 */

void Scene::HandleInputs(MSG &msg)
{
    switch (msg.message)
    {
    case WM_KEYDOWN:

        if (msg.wParam == 0x46)
        {
            if (wireFrame == false)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                wireFrame = true;
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                wireFrame = false;
            }
        }

        cam.HandleKeyDown(msg.wParam);
        break;

    case WM_KEYUP:

        cam.HandleKeyUp(msg.wParam);
        break;

    case WM_MOUSEMOVE:

        cam.HandleMouseMove(msg.lParam);
        break;

    case WM_LBUTTONDOWN:

        cam.HandleMouseDown(msg.lParam);
        break;

    case WM_LBUTTONUP:

        cam.HandleMouseUp();
        break;

    default:
        break;
    }
}