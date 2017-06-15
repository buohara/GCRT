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

    LoadSettings("settings.txt");

    nextPickClr[0] = 0.0f;
    nextPickClr[1] = 0.0f;
    nextPickClr[2] = 0.1f;
    nextPickIdx = 2;
    t = 0.0;

    mousePos[0] = 0.0;
    mousePos[1] = 0.0;
    mouseDown[0] = false;
    mouseDown[1] = false;
    mouseDown[2] = false;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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

    pickerPass.Init(settings.winW, settings.winH);
    depthPass.Init();

    renderPass.Init(
        depthPass.getDepthTex(),
        renderFbo,
        settings.winW,
        settings.winH,
        false,
        settings.msaaSamples
    );

    dofPass.Init(
        renderTex,
        scn.diffTextures["NoiseTex"].texID,
        renderFbo,
        settings.winW,
        settings.winH
    );

    bloomPass.Init(
        renderTex,
        RENDER_TO_SCREEN,
        settings.winW,
        settings.winH
    );

    if (settings.loadSceneFromFile)
    {
        scn.Load(settings.sceneName);
        for (uint32_t i = 0; i < scn.models.size(); i++)
        {
            nextPickerColor();
        }
    }
    else
    {
        LoadTextures();
        InitMaterials();
        InitModels();
        InitLights();
    }
}

/**
 * Create in/out FBO/texture to hand between passes.
 */

vec3 Renderer::nextPickerColor()
{
    vec3 retVec = vec3(nextPickClr[0], nextPickClr[1], nextPickClr[2]);
    if (nextPickIdx == 0)
    {
        nextPickIdx = 2;
    }
    else
    {
        nextPickIdx--;
    }

    nextPickClr[nextPickIdx] += 0.1f;

    return retVec;
}

/**
 * Create in/out FBO/texture to hand between passes.
 */

void Renderer::UpdateViewPorts(uint32_t w, uint32_t h)
{
    scn.cam.aspect = (float)w / (float)h;

    settings.winW = w;
    settings.winH = h;
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, settings.winW, settings.winH, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Depth attachment

    GLuint depthRenderBuffer;
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, settings.winW, settings.winH);

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
    scn.AddDiffTexture(
        "DirtDiffuse",
        string("../asset/dirtdiffuse.jpg"),
        ImgLoader::LoadTexture(string("../asset/dirtdiffuse.jpg"))
    );

    scn.AddNormTexture(
        "DirtNormal",
        string("../asset/dirtnormal.jpg"),
        ImgLoader::LoadTexture(string("../asset/dirtnormal.jpg"))
    );
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
    RMaterial defaultMat;
    defaultMat.name = "Default";
    defaultMat.kd = vec3(0.8, 0.8, 0.8);
    defaultMat.UseShadows(true);
    defaultMat.spec = 1.0;
    defaultMat.useSSS = true;
    scn.AddMaterial("Default", defaultMat);
}

/**
 * Create models from geometries and materials.
 */

void Renderer::InitModels()
{
    // Plane

    Plane pln;
    pln.Create(10, 10);
    pln.name = "Plane";
    pln.loadFromFile = false;
    pln.filePath = "NA";
    scn.AddMesh("Plane", make_shared<Plane>(pln));

    Model plane;
    plane.meshName = string("Plane");
    plane.matName = string("Default");
    plane.pickerColor = nextPickerColor();
    plane.InitModelMatrices();
    plane.Scale(vec3(10.0, 10.0, 1.0));
    scn.AddModel("Plane0", plane);

    scn.LoadModelFromFile(
        "Bones",
        "../asset/models/boblampclean/boblampclean.md5mesh",
        "",
        "",
        nextPickerColor(),
        false
    );
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

    t += 0.05f;
    if (t > 30.0)
    {
        t = 0.0;
    }
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

        if (msg.wParam == 0x54) // 't' key to kick off ray trace.
        {
            
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
    uint32_t y = settings.winH - (GET_Y_LPARAM(mouseCoord) + 40);

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
            (*it).second.selected = true;

            if (firstHit == true)
            {
                firstHit = false;
            }
            else
            {
                if ((*it).first != selected)
                {
                    scn.models[selected].selected = false;
                }
            }

            selected = (*it).first;
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

    scn.AddDiffTexture("NoiseTex", "", noiseTexID);
}

/**
 * Copy current scene state to ray tracer, then kickoff another thread to
 * do the render.
 */

void Renderer::KickoffRayTrace()
{    
    rtIn.models         = scn.models;
    rtIn.diffTextures   = scn.diffTextures;
    rtIn.normTextures   = scn.normTextures;
    rtIn.materials      = scn.materials;
    rtIn.dirLights      = scn.dirLights;
    rtIn.ptLights       = scn.ptLights;
    rtIn.meshes         = scn.meshes;

    CreateThread(
        NULL,
        0,
        RayTracer::RTThreadFunc,
        &rtIn,
        0,
        &rtThreadId
    );
}

/**
 * LoadSettings -
 */

void Renderer::LoadSettings(string file)
{
    ifstream fin;
    fin.open(file);

    string line;
    istringstream iss;

    uint32_t winW;
    uint32_t winH;
    bool wireFrame;
    bool useDOF;
    bool useBloom;
    uint32_t msaaSamples;
    bool loadSceneFromFile;
    string scene;

    getline(fin, line);
    getline(fin, line);
    iss.str(line);
    iss >> winW >> winH >> wireFrame >> useDOF >> useBloom >> msaaSamples >> loadSceneFromFile >> scene;

    settings.winW = winW;
    settings.winH = winH;
    settings.wireFrame = wireFrame;
    settings.useDOF = useDOF;
    settings.useBloom = useBloom;
    settings.msaaSamples = msaaSamples;
    settings.loadSceneFromFile = loadSceneFromFile;
    settings.sceneName = scene;

    fin.close();
}