#include "renderer.h"

/**
* Init - Initialize scene assets.
*/

void Renderer::Init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glCullFace(GL_BACK);

    settings.useDOF = false;
    settings.useBloom = true;
    settings.msaaSamples = 1;

    winW = 1920;
    winH = 1080;

    mousePos[0] = 0.0;
    mousePos[1] = 0.0;
    mouseDown[0] = false;
    mouseDown[1] = false;
    mouseDown[2] = false;

    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    glClearDepth(1.0f);

    scn.cam.Init(
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
        false,
        settings.msaaSamples
    );

    dofPass.Init(
        renderTex,
        scn.textures["NoiseTex"],
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

void Renderer::UpdateViewPorts(uint32_t w, uint32_t h)
{
    scn.cam.aspect = (float)w / (float)h;

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

void Renderer::ResizeRenderFbo()
{
    glDeleteFramebuffers(1, &renderFbo);
    glDeleteTextures(1, &renderTex);
    CreateRenderPassFbo();
}

/**
* Create in/out FBO/texture to hand between passes.
*/

void Renderer::CreateRenderPassFbo()
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

void Renderer::LoadTextures()
{
    scn.textures["DirtDiffuse"] =
        ImgLoader::LoadTexture(string("../asset/dirtdiffuse.jpg"));

    scn.textures["DirtNormal"] =
        ImgLoader::LoadTexture(string("../asset/dirtnormal.jpg"));

    scn.textures["GrassDiffuse"] =
        ImgLoader::LoadTexture(string("../asset/grassdiffuse.jpg"));

    scn.textures["GrassNormal"] =
        ImgLoader::LoadTexture(string("../asset/grassNormal.jpg"));
}

/**
* InitLights - Initialize scene lights.
*/

void Renderer::InitLights()
{
    DirectionalLight dirLight;
    dirLight.pos = vec3(0.0, 25.0, 25.0);
    dirLight.look = vec3(0.0, 0.0, 0.0);
    scn.dirLights.push_back(dirLight);

    PointLight ptLight;
    ptLight.pos = vec3(0.0, 15.0, 15.0);
    scn.ptLights.push_back(ptLight);
}

/**
* InitMaterials - Build materials from shaders and textures.
*/

void Renderer::InitMaterials()
{
    RMaterial dirtMat;
    dirtMat.name = "Dirt";
    dirtMat.SetDiffuseTex(scn.textures["DirtDiffuse"]);
    dirtMat.SetNormalTex(scn.textures["DirtNormal"]);
    dirtMat.UseShadows(true);
    dirtMat.spec = 2.0;
    scn.materials["Dirt"] = dirtMat;

    RMaterial redMat;
    redMat.name = "RedMat";
    redMat.kd = vec3(12.0, 0.4, 0.4);
    redMat.UseShadows(true);
    redMat.SetNormalTex(scn.textures["DirtNormal"]);
    redMat.spec = 2.0;
    scn.materials["RedMat"] = redMat;

    RMaterial greenMat;
    greenMat.name = "GreenMat";
    greenMat.kd = vec3(0.4, 3.9, 0.4);
    greenMat.UseShadows(true);
    greenMat.SetNormalTex(scn.textures["DirtNormal"]);
    greenMat.spec = 2.0;
    scn.materials["GreenMat"] = greenMat;

    RMaterial blueMat;
    blueMat.name = "BlueMat";
    blueMat.kd = vec3(0.1, 0.4, 4.4);
    blueMat.UseShadows(true);
    blueMat.SetNormalTex(scn.textures["DirtNormal"]);
    blueMat.spec = 2.0;
    scn.materials["BlueMat"] = blueMat;

    RMaterial yellowMat;
    yellowMat.name = "YellowMat";
    yellowMat.kd = vec3(0.8, 0.8, 0.01);
    yellowMat.UseShadows(true);
    yellowMat.SetNormalTex(scn.textures["DirtNormal"]);
    yellowMat.spec = 2.0;
    scn.materials["YellowMat"] = yellowMat;
}

/**
* Create models from geometries and materials.
*/

void Renderer::InitModels()
{
    // Plane

    Plane pln;
    pln.Create(10, 10);
    pln.Scale(vec3(10.0, 10.0, 1.0));

    scn.models["Plane"].pGeom = make_shared<Plane>(pln);
    scn.models["Plane"].SetMaterial(scn.materials["Dirt"]);
    scn.models["Plane"].pickerColor = vec3(0.0, 0.0, 0.1);

    Sphere sph1;
    sph1.Create(50, 50);
    sph1.Scale(vec3(2.0, 2.0, 2.0));
    sph1.Translate(vec3(10.0, 0.0, 3.0));

    scn.models["Sphere1"].pGeom = make_shared<Sphere>(sph1);
    scn.models["Sphere1"].SetMaterial(scn.materials["RedMat"]);
    scn.models["Sphere1"].pickerColor = vec3(0.0, 0.1, 0.1);

    Sphere sph2;
    sph2.Create(50, 50);
    sph2.Scale(vec3(2.0, 2.0, 2.0));
    sph2.Translate(vec3(7.07, 7.07, 3.0));

    scn.models["Sphere2"].pGeom = make_shared<Sphere>(sph2);
    scn.models["Sphere2"].SetMaterial(scn.materials["GreenMat"]);
    scn.models["Sphere2"].pickerColor = vec3(0.1, 0.1, 0.1);

    Sphere sph3;
    sph3.Create(50, 50);
    sph3.Scale(vec3(2.0, 2.0, 2.0));
    sph3.Translate(vec3(0.0, 10.0, 3.0));

    scn.models["Sphere3"].pGeom = make_shared<Sphere>(sph3);
    scn.models["Sphere3"].SetMaterial(scn.materials["YellowMat"]);
    scn.models["Sphere3"].pickerColor = vec3(0.1, 0.1, 0.2);

    Sphere sph4;
    sph4.Create(50, 50);
    sph4.Scale(vec3(2.0, 2.0, 2.0));
    sph4.Translate(vec3(-7.07, 7.07, 3.0));

    scn.models["Sphere4"].pGeom = make_shared<Sphere>(sph4);
    scn.models["Sphere4"].SetMaterial(scn.materials["BlueMat"]);
    scn.models["Sphere4"].pickerColor = vec3(0.1, 0.2, 0.2);

    Sphere sph5;
    sph5.Create(50, 50);
    sph5.Scale(vec3(2.0, 2.0, 2.0));
    sph5.Translate(vec3(-10.0, 0.0, 3.0));

    scn.models["Sphere5"].pGeom = make_shared<Sphere>(sph5);
    scn.models["Sphere5"].SetMaterial(scn.materials["RedMat"]);
    scn.models["Sphere5"].pickerColor = vec3(0.2, 0.2, 0.2);

    Sphere sph6;
    sph6.Create(50, 50);
    sph6.Scale(vec3(2.0, 2.0, 2.0));
    sph6.Translate(vec3(-7.07, -7.07, 3.0));

    scn.models["Sphere6"].pGeom = make_shared<Sphere>(sph6);
    scn.models["Sphere6"].SetMaterial(scn.materials["GreenMat"]);
    scn.models["Sphere6"].pickerColor = vec3(0.2, 0.2, 0.3);

    Sphere sph7;
    sph7.Create(50, 50);
    sph7.Scale(vec3(2.0, 2.0, 2.0));
    sph7.Translate(vec3(0.0, -10.0, 3.0));

    scn.models["Sphere7"].pGeom = make_shared<Sphere>(sph7);
    scn.models["Sphere7"].SetMaterial(scn.materials["YellowMat"]);
    scn.models["Sphere7"].pickerColor = vec3(0.2, 0.3, 0.3);

    Sphere sph8;
    sph8.Create(50, 50);
    sph8.Scale(vec3(2.0, 2.0, 2.0));
    sph8.Translate(vec3(7.07, -7.07, 3.0));

    scn.models["Sphere8"].pGeom = make_shared<Sphere>(sph8);
    scn.models["Sphere8"].SetMaterial(scn.materials["BlueMat"]);
    scn.models["Sphere8"].pickerColor = vec3(0.3, 0.3, 0.3);
}

/**
 * Render - Loop through each model in the scene and draw it.
 */

void Renderer::Render(HDC hDC)
{
    scn.cam.Update();
    
    pickerPass.Render(scn);
    depthPass.Render(scn);
    renderPass.Render(scn);

    if (settings.useDOF == true)
    {
        dofPass.Render();
    }

    if (settings.useBloom == true)
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

void Renderer::HandleInputs(MSG &msg)
{
    ImGuiIO& io = ImGui::GetIO();

    switch (msg.message)
    {
    case WM_CHAR:

        io.AddInputCharacter((ImWchar)msg.wParam);
        break;

    case WM_KEYDOWN:

        if (msg.wParam == 0x46) // 'f' key for wireframe.
        {
            settings.wireFrame = !settings.wireFrame;
            renderPass.wireFrame = settings.wireFrame;
        }

        scn.cam.HandleKeyDown(msg.wParam);

        if (msg.wParam == 0x10) // IMGUI 'shift' modifier shouldn't be cleared until released.
        {
            ImGuiGCRTSetKey((int)msg.wParam, 1);
        }
        else
        {
            ImGuiKeys.push_back((uint32_t)msg.wParam);
        }
        break;

    case WM_KEYUP:

        if (msg.wParam == 0x10) // Clear IMGUI 'shift' modifier.
        {
            ImGuiGCRTSetKey((int)msg.wParam, 0);
        }

        scn.cam.HandleKeyUp(msg.wParam);
        break;

    case WM_MOUSEMOVE:

        if (io.WantCaptureMouse == false)
        {
            scn.cam.HandleMouseMove(msg.lParam);
        }
        mousePos[0] = (double)GET_X_LPARAM(msg.lParam);
        mousePos[1] = (double)GET_Y_LPARAM(msg.lParam) + 40.0;
        break;

    case WM_LBUTTONDOWN:

        if (io.WantCaptureMouse == false)
        {
            scn.cam.HandleMouseDown(msg.lParam);
            DoPick(msg.lParam);
        }

        mouseDown[0] = true;
        mousePos[0] = (double)GET_X_LPARAM(msg.lParam);
        mousePos[1] = (double)GET_Y_LPARAM(msg.lParam) + 40.0;
        break;

    case WM_LBUTTONUP:

        scn.cam.HandleMouseUp();
        mouseDown[0] = false;
        break;

    default:
        break;
    }
}

/**
 * DoHitTest
 */

void Renderer::DoPick(LPARAM mouseCoord)
{
    uint32_t x = GET_X_LPARAM(mouseCoord);
    uint32_t y = winH - (GET_Y_LPARAM(mouseCoord) + 40);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, pickerPass.pickerFboID);

    float pixel4[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &pixel4[0]);
    vec3 pixel = vec3(pixel4[0], pixel4[1], pixel4[2]);

    map<string, Model>::iterator it;
    static bool firstHit = true;

    for (it = scn.models.begin(); it != scn.models.end(); it++)
    {
        vec3 pickerColor = (*it).second.pickerColor;
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
                if ((*it).first != selected.name)
                {
                    scn.models[selected.name].mat.selected = false;
                }
            }

            selected.name = (*it).first;
            selected.kd = (*it).second.mat.kd;
            selected.pos = (*it).second.pGeom->pos;
            selected.scale = (*it).second.pGeom->dims;
            selected.specular = (*it).second.mat.spec;
        }
    }
}

/**
* CreateNoiseTexture - Create a texture with random gray-scale pixel values.
*/

void Renderer::CreateNoiseTexture()
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

    scn.textures["NoiseTex"] = noiseTexID;
}