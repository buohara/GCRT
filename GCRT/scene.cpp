#include "scene.h"

/**
 * Init - Initialize scene assets.
 */

void Scene::Init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glCullFace(GL_BACK);
    
    useDOF = true;
    useBloom = true;

    winW = 2560;
    winH = 1440;

    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    glClearDepth(1.0f);

    cam.Init(
        vec3(15.0, 15.0, 15.0),
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        4.0f / 3.0f,
        75.0f,
        1.0f,
        100.f
    );

    CreateNoiseTexture();
    CreateRenderPassFbo();

    depthPass.Init();
    
    renderPass.Init(
        depthPass.getDepthTex(), 
        renderFbo,
        winW,
        winH,
        true
    );
    
    dofPass.Init(
        renderTex,
        textures["NoiseTex"],
        renderFbo,
        winW,
        winH
    );

    bloomPass.Init(
        renderTex,
        RENDER_TO_SCREEN,
        winW,
        winH
    );

    LoadTextures();
    InitMaterials();
    InitModels();
    InitLights();
}

/**
 * Create in/out FBO/texture to hand between passes.
 */

void Scene::CreateRenderPassFbo()
{
    glGenFramebuffers(1, &renderFbo);

    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, winW, winH, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, winW, winH);

    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
 * InitLights - Initialize scene lights.
 */

void Scene::InitLights()
{
    DirectionalLight dirLight;
    dirLight.pos = vec3(0.0, 25.0, 25.0);
    dirLight.look = vec3(0.0, 0.0, 0.0);
    dirLights.push_back(dirLight);

    PointLight ptLight;
    ptLight.pos = vec3(0.0, 15.0, 15.0);
    ptLights.push_back(ptLight);
}

/**
 * InitMaterials - Build materials from shaders and textures.
 */

void Scene::InitMaterials()
{
    // Matte green SSS material with bumps.

    RMaterial dirtMat;
    dirtMat.name = "Dirt";
    dirtMat.SetDiffuseTex(textures["DirtDiffuse"]);
    dirtMat.SetNormalTex(textures["DirtNormal"]);
    dirtMat.UseShadows(true);
    materials["Dirt"] = dirtMat;

    RMaterial redMat;
    redMat.name = "RedMat";
    redMat.kd = vec3(12.0, 0.4, 0.4);
    redMat.UseShadows(true);
    redMat.SetNormalTex(textures["DirtNormal"]);
    materials["RedMat"] = redMat;

    RMaterial greenMat;
    greenMat.name = "GreenMat";
    greenMat.kd = vec3(0.4, 3.9, 0.4);
    greenMat.UseShadows(true);
    materials["GreenMat"] = greenMat;

    RMaterial yellowMat;
    yellowMat.name = "YellowMat";
    yellowMat.kd = vec3(0.8, 0.8, 0.01);
    yellowMat.UseShadows(true);
    materials["YellowMat"] = yellowMat;
}

/**
 * Create models from geometries and materials.
 */

void Scene::InitModels()
{
    // Plane

    Plane pln;
    pln.Create(10, 10);
    pln.Scale(vec3(10.0, 10.0, 1.0));

    models["Plane"].pGeom = make_shared<Plane>(pln);
    models["Plane"].SetMaterial(materials["GreenMat"]);

    Box box;
    box.Create();
    box.Scale(vec3(2.0, 2.0, 2.0));
    box.Translate(vec3(2.0, 0.0, 1.0));

    models["Box"].pGeom = make_shared<Box>(box);
    models["Box"].SetMaterial(materials["GreenMat"]);

    Sphere sph;
    sph.Create(50, 50);
    sph.Scale(vec3(4.0, 4.0, 4.0));
    sph.Translate(vec3(0.0, 12.0, 3.0));

    models["Sphere"].pGeom = make_shared<Sphere>(sph);
    models["Sphere"].SetMaterial(materials["RedMat"]);

    Cylinder cyl;
    cyl.Create(25);
    cyl.Scale(vec3(2.0, 2.0, 3.0));
    cyl.Translate(vec3(0.0, -30.0, 3.0));

    models["Cylinder"].pGeom = make_shared<Cylinder>(cyl);
    models["Cylinder"].SetMaterial(materials["YellowMat"]);
}

/**
 * Render - Loop through each model in the scene and draw it.
 */

void Scene::Render(HDC hDC)
{
    static float t = 0;

    // Update camera from inputs.

    cam.Update();

    depthPass.Render(models, dirLights);
    
    renderPass.Render(
        models,
        cam,
        dirLights,
        ptLights
    );
    
    if (useDOF == true)
    {
        dofPass.Render();
    }

    if (useBloom == true)
    {
        bloomPass.Render();
    }

    // Swap.

    SwapBuffers(hDC);
    t += 0.005f;
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
        DoHitTest(msg.lParam);
        break;

    case WM_LBUTTONUP:

        cam.HandleMouseUp();
        break;

    default:
        break;
    }
}

/**
 * DoHitTest
 */

void Scene::DoHitTest(LPARAM mouseCoord)
{
    vec4 p;
    p.x = (float)GET_X_LPARAM(mouseCoord);
    p.y = (float)GET_Y_LPARAM(mouseCoord);

    float screenW = (float)winW;
    float screenH = (float)winH;

    p.x = 2.0f * p.x / winW - 1.0;
    p.y = 1.0f - 2.0f * p.y / winH;
    p.z = -1.0f;
    p.w = 1.0f;

    mat4 projInv = inverse(cam.GetProjection());
    mat4 viewInv = inverse(cam.GetView());
    vec3 camPos = cam.pos;

    p = projInv * p;
    p.z = -1.0;
    p.w = 0.0;

    vec4 ray = normalize(viewInv * p);

    map<string, Model>::iterator it;
    map<string, Model>::iterator closest;

    float minDist = 1000.0f;
    bool hit = false;

    for (it = models.begin(); it != models.end(); it++)
    {
        float dist = (*it).second.pGeom->Intersect(camPos, ray);
        
        if (dist > 0.0 && dist < minDist)
        {
            closest = it;
            minDist = dist;
            hit = true;
        }
        else
        {
            (*it).second.mat.selected = 0;
        }
    }

    if (hit == true)
    {
        (*closest).second.mat.selected = 1;
    }
}

/**
 * CreateNoiseTexture - Create a texture with random gray-scale pixel values.
 */

void Scene::CreateNoiseTexture()
{
    uint32_t w = 1024;
    uint32_t h = 1024;

    vector<BYTE> pixels;
    pixels.resize(w * h);

    for (uint32 i = 0; i < w * h; i++)
    {
        pixels[i] = (BYTE)(256 * (float)rand() / (float)RAND_MAX);
    }

    GLuint noiseTexID;
    glGenTextures(1, &noiseTexID);
    glBindTexture(GL_TEXTURE_2D, noiseTexID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_BYTE, &pixels[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    textures["NoiseTex"] = noiseTexID;
}