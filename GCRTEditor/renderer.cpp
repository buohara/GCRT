#include "renderer.h"

#define GLEW_STATIC

/**
 * Renderer::CreateGLContext Create OpenGL context and swapchain.
 *
 * @param  hWnd Window handle for this app.
 * @return Handle to GL context.
 */

void Renderer::CreateGLContext()
{
    // Swapchain format format.

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize       = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = 32;
    pfd.cDepthBits  = 32;
    pfd.iLayerType  = PFD_MAIN_PLANE;

    hDC = GetDC(hWnd);

    int pxFmt;
    pxFmt = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pxFmt, &pfd);

    // Dummy 2.1 context.

    HGLRC hDummyCtx = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hDummyCtx);

    GLenum err = glewInit();

    if (err != GLEW_OK)
    {
        MessageBoxA(0, "Coult not initialize GLEW.", "Error", 0);
    }

    // Real 3.1 context.

    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        WGL_CONTEXT_FLAGS_ARB, 0,
        0
    };

    HGLRC hCtx = wglCreateContextAttribsARB(hDC, 0, attribs);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hDummyCtx);
    wglMakeCurrent(hDC, hCtx);
}

/**
 * Renderer::CreateRenderWindow Create a windows window for rendering.
 */

void Renderer::CreateRenderWindow(
    HINSTANCE hInstance,
    uint32_t width,
    uint32_t height,
    string title
)
{
    WNDCLASS wc         = { 0 };
    wc.lpfnWndProc      = WndProc;
    wc.hInstance        = hInstance;
    wc.hbrBackground    = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName    = "GLWindow";
    wc.style            = CS_OWNDC;

    if (!RegisterClass(&wc))
    {
        return;
    }

    // Create the main GL Window.

    hWnd = CreateWindowW(
        L"GLWindow",
        L"GLWindow",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        0,
        0,
        width,
        height,
        0,
        0,
        hInstance,
        0
    );

    DWORD err = GetLastError();
    return;
}

/**
 * Renderer::Init Parse renderer settings from file and initialize
 * camera and render passes.
 */

void Renderer::Init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glCullFace(GL_BACK);

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

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

    if (settings.loadSceneFromFile)
    {
        scn.Load(settings.sceneName);
        for (auto &model : scn.models)
        {
            nextPickerColor();
        }
    }
    else
    {
        LoadDefaultScene();
    }
}

/**
 * Renderer::LoadDefaultScene Load default scene (a plane with knight character).
 */

void Renderer::LoadDefaultScene()
{
    LoadDefaultTextures();
    InitDefaultMaterials();
    InitDefaultModels();
    InitDefaultLights();
}

/**
 * Renderer::nextPickerColor For picking objects on screen, every object
 * is assigned a unique color and rendered to a picker buffer, which is then
 * read back on mouse clicks. Assign picker colors with this function.
 *
 * @return New picker color.
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
 * Renderer::UpdateViewPorts On window resizes, update render pass
 * resources appropriately.
 *
 * @param w New window width.
 * @param h New window height.
 */

void Renderer::UpdateViewPorts(uint32_t w, uint32_t h)
{
    scn.cam.aspect = (float)w / (float)h;
    settings.winW = w;
    settings.winH = h;
}

/**
 * Renderer::ResizeRenderFbo Resize main render target that will be presented to
 * screen.
 */

void Renderer::ResizeRenderFbo()
{
    glDeleteFramebuffers(1, &renderFbo);
    glDeleteTextures(1, &renderTex);
    CreateRenderPassFbo();
}

/**
 * Renderer::LoadTextures Load default scene textures.
 */

void Renderer::LoadDefaultTextures()
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
 * Renderer::InitLights Create default scene lights.
 */

void Renderer::InitDefaultLights()
{
    DirectionalLight dirLight;
    dirLight.pos = vec3(0.0, -25.0, 25.0);
    dirLight.look = vec3(0.0, 0.0, 0.0);
    scn.dirLights.push_back(dirLight);

    PointLight ptLight;
    ptLight.pos = vec3(0.0, 15.0, 15.0);
    scn.ptLights.push_back(ptLight);
}

/**
 * Renderer::InitMaterials Create default scene materials.
 */

void Renderer::InitDefaultMaterials()
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
 * Renderer::InitModels Create default scene models.
 */

void Renderer::InitDefaultModels()
{
    Plane pln;
    pln.Create(10, 10);
    pln.name = "Plane";
    pln.loadFromFile = false;
    pln.filePath = "NA";

    scn.AddMesh("Plane", make_shared<Plane>(pln));

    Model plane;
    plane.InitModelMatrices();
    plane.Scale(vec3(10.0, 10.0, 1.0));
    plane.meshName = string("Plane");
    plane.matName = string("Default");
    plane.pickerColor = nextPickerColor();
    scn.AddModel("Plane0", plane);

    scn.LoadModelFromFile(
        "LampGuy",
        "../asset/models/boblampclean/boblampclean.md5mesh",
        "",
        "",
        nextPickerColor(),
        false
    );
}

/**
 * Renderer::Render Main rendering routine. Update camera, model animations,
 * and execute each render pass.
 *
 * @param hDC Device context handle (for flipping back buffers).
 */

void Renderer::Render()
{
    scn.cam.Update();
    
    for (auto &model : scn.models)
    {
        auto pMesh = scn.meshes[model.second.meshName];
        model.second.UpdateAnimation(t, pMesh);
    }

    for (auto &pass : passes)
    {
        pass->Render(scn);
    }

    SwapBuffers(hDC);

    t += 0.25f;
    if (t > 150.0)
    {
        t = 0.0;
    }
}

/**
 * Renderer::HandleInputs Handle keyboard and mouse inputs.
 * @param msg Input message.
 */

void Renderer::HandleInputs(MSG &msg)
{
    switch (msg.message)
    {

    case WM_KEYDOWN:

        if (msg.wParam == 0x46) // 'f' key for wireframe.
        {
            settings.wireFrame = !settings.wireFrame;
            renderPass.wireFrame = settings.wireFrame;
        }

        scn.cam.HandleKeyDown(msg.wParam);

    case WM_KEYUP:

        scn.cam.HandleKeyUp(msg.wParam);
        break;

    case WM_MOUSEMOVE:

        scn.cam.HandleMouseMove(msg.lParam);
        mousePos[0] = (double)GET_X_LPARAM(msg.lParam);
        mousePos[1] = (double)GET_Y_LPARAM(msg.lParam) + 40.0;
        break;

    case WM_LBUTTONDOWN:

        scn.cam.HandleMouseDown(msg.lParam);
        DoPick(msg.lParam);
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
 * Renderer::DoPick On mouse click, read pixel color from (x, y) coordinates
 * and find object in scene with matching color.
 *
 * @param mouseCoord (x, y) coordinates of mouse click.
 */

void Renderer::DoPick(LPARAM mouseCoord)
{
    uint32_t x = GET_X_LPARAM(mouseCoord);
    uint32_t y = settings.winH - (GET_Y_LPARAM(mouseCoord) + 40);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, pickerPass.pickerFboID);

    float pixel4[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &pixel4[0]);
    vec3 pixel = vec3(pixel4[0], pixel4[1], pixel4[2]);

    static bool firstHit = true;

    for (auto &model : scn.models)
    {
        vec3 pickerColor = model.second.pickerColor;
        if (abs(pickerColor.x - pixel.x) < 0.05 &&
            abs(pickerColor.y - pixel.y) < 0.05 &&
            abs(pickerColor.z - pixel.z) < 0.05)
        {
            model.second.selected = true;

            if (firstHit == true)
            {
                firstHit = false;
            }
            else
            {
                if (model.first != selected)
                {
                    scn.models[selected].selected = false;
                }
            }

            selected = model.first;
        }
    }
}

/**
 * Renderer::CreateNoiseTexture Create a texture with random pixel values
 * for effects that require random samples at each pixel.
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