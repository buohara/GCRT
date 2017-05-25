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
    glEnable(GL_MULTISAMPLE);
    
    useDOF = false;
    useBloom = true;

    winW = 1920;
    winH = 1080;

    mousePos[0] = 0.0;
    mousePos[1] = 0.0;
    mouseDown[0] = false;
    mouseDown[1] = false;
    mouseDown[2] = false;

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

    pickerPass.Init(winW, winH);
    depthPass.Init();
    
    renderPass.Init(
        depthPass.getDepthTex(), 
        renderFbo,
        winW,
        winH,
        false
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

void Scene::UpdateViewPorts(uint32_t w, uint32_t h)
{
    cam.aspect = (float)w / (float)h;

    winW = w;
    winH = h;
    renderPass.fboWidth = w;
    renderPass.fboHeight = h;
    ResizeRenderFbo();

    ImGuiGCRTResize(w, h);
    pickerPass.Resize(w, h);
    bloomPass.Resize(w, h);
}

/**
 * ResizeRenderFbo
 */

void Scene::ResizeRenderFbo()
{
    glDeleteFramebuffers(1, &renderFbo);
    glDeleteTextures(1, &renderTex);
    CreateRenderPassFbo();
}

/**
 * Create in/out FBO/texture to hand between passes.
 */

void Scene::CreateRenderPassFbo()
{
    glGenFramebuffers(1, &renderFbo);

    // Output texture.

    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, winW, winH, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Depth attachment

    GLuint depthRenderBuffer;
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, winW, winH);

    // Attach 

    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
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
    RMaterial dirtMat;
    dirtMat.name = "Dirt";
    dirtMat.SetDiffuseTex(textures["DirtDiffuse"]);
    dirtMat.SetNormalTex(textures["DirtNormal"]);
    dirtMat.UseShadows(true);
    dirtMat.pickerColor = vec3(0.1, 0.1, 0.1);
    materials["Dirt"] = dirtMat;

    RMaterial redMat;
    redMat.name = "RedMat";
    redMat.kd = vec3(12.0, 0.4, 0.4);
    redMat.UseShadows(true);
    redMat.SetNormalTex(textures["DirtNormal"]);
    redMat.pickerColor = vec3(0.2, 0.2, 0.2);
    materials["RedMat"] = redMat;

    RMaterial greenMat;
    greenMat.name = "GreenMat";
    greenMat.kd = vec3(0.4, 3.9, 0.4);
    greenMat.UseShadows(true);
    greenMat.pickerColor = vec3(0.3, 0.3, 0.3);
    materials["GreenMat"] = greenMat;

    RMaterial yellowMat;
    yellowMat.name = "YellowMat";
    yellowMat.kd = vec3(0.8, 0.8, 0.01);
    yellowMat.UseShadows(true);
    yellowMat.pickerColor = vec3(0.4, 0.4, 0.4);
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
    models["Plane"].SetMaterial(materials["Dirt"]);

    Box box;
    box.Create();
    box.Scale(vec3(1.0, 1.0, 1.0));
    box.Translate(vec3(10.0, 0.0, 1.0));

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
 * UpdateImGui - 
 */

void Scene::UpdateImGui()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGuiGCRTSetMouse(
        mousePos[0],
        mousePos[1],
        mouseDown[0],
        mouseDown[1],
        mouseDown[2]
    );

    ImGuiGCRTNewFrame();

    static bool show_test_window;
    static ImVec4 diffuse_color = ImColor(0, 0, 0);
    static float specular = 0.0;
    static ImVec4 pos(0, 0, 0, 1);
    static ImVec4 scale(0, 0, 0, 1);

    // Model Properties Window

    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Model Properties");

    ImGui::Text("Material");
    
    ImGui::InputFloat3("DiffuseColor", (float*)&diffuse_color);
    
    ImGui::InputFloat("Specular", (float*)&specular);
    
    ImGui::Text("Geometry");
    ImGui::InputFloat3("Pos", (float*)&pos);
    ImGui::InputFloat3("Scale", (float*)&scale);
    
    if (ImGui::Button("Test Window")) show_test_window ^= 1;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_test_window);
    }

    ImGui::Render();
    int i = 0;
    i++;
}

/**
 * Render - Loop through each model in the scene and draw it.
 */

void Scene::Render(HDC hDC)
{
    cam.Update();
    pickerPass.Render(models, cam);
    
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

    UpdateImGui();

    SwapBuffers(hDC);
}

/**
 * HandleInputs - Process any keyboard and mouse inputs.
 *
 * @param msg Windows event message to handle.
 */

void Scene::HandleInputs(MSG &msg)
{
    ImGuiIO& io = ImGui::GetIO();

    switch (msg.message)
    {
    case WM_CHAR:

        io.AddInputCharacter(msg.wParam);
        break;

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

        if (io.WantCaptureMouse == false)
        {
            cam.HandleMouseMove(msg.lParam);
        }
        mousePos[0] = (double)GET_X_LPARAM(msg.lParam);
        mousePos[1] = (double)GET_Y_LPARAM(msg.lParam) + 40.0;
        break;

    case WM_LBUTTONDOWN:

        if (io.WantCaptureMouse == false)
        {
            cam.HandleMouseDown(msg.lParam);
            DoPick(msg.lParam);
        }

        mouseDown[0] = true;
        mousePos[0] = (double)GET_X_LPARAM(msg.lParam);
        mousePos[1] = (double)GET_Y_LPARAM(msg.lParam) + 40.0;
        break;

    case WM_LBUTTONUP:

        cam.HandleMouseUp();
        mouseDown[0] = false;
        break;

    default:
        break;
    }
}

/**
 * DoHitTest
 */

void Scene::DoPick(LPARAM mouseCoord)
{
    uint32_t x = GET_X_LPARAM(mouseCoord);
    uint32_t y = winH - (GET_Y_LPARAM(mouseCoord) + 40.0);
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, pickerPass.pickerFboID);

    float pixel4[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &pixel4[0]);
    vec3 pixel = vec3(pixel4[0], pixel4[1], pixel4[2]);

    map<string, Model>::iterator it;

    static map<string, Model>::iterator lastHit;
    static bool firstHit = true;

    for (it = models.begin(); it != models.end(); it++)
    {
        vec3 pickerColor = (*it).second.mat.pickerColor;
        if (abs(pickerColor.x - pixel.x) < 0.05 &&
            abs(pickerColor.y - pixel.y) < 0.05 &&
            abs(pickerColor.z - pixel.z) < 0.05)
        {
            (*it).second.mat.selected = true;

            if (firstHit == true)
            {
                firstHit = false;
            }
            else
            {
                if (it != lastHit)
                {
                    (*lastHit).second.mat.selected = false;
                }
            }
            lastHit = it;
        }
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