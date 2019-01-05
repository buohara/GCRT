#include "renderer.h"

#define GLEW_STATIC

RenderSettings g_settings = { 0 };
Scene g_scn = {};

static long long GetMilliseconds()
{
    static LARGE_INTEGER frequency;
    static BOOL useQpc = QueryPerformanceFrequency(&frequency);

    if (useQpc)
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / frequency.QuadPart;
    }
    else
    {
        return GetTickCount();
    }
}

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
        g_settings.winW,
        g_settings.winH,
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

    nextPickClr[0]  = 0.0f;
    nextPickClr[1]  = 0.0f;
    nextPickClr[2]  = 0.1f;
    nextPickIdx     = 2;
    t1              = (float)GetMilliseconds() / 1000.f;
    t2              = t1;

    mousePos[0]     = 0.0;
    mousePos[1]     = 0.0;
    mouseDown[0]    = false;
    mouseDown[1]    = false;
    mouseDown[2]    = false;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClearDepth(1.0f);

    CreateNoiseTexture();
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

void Renderer::UpdateViewPorts()
{
    uint32_t w = g_settings.winW;
    uint32_t h = g_settings.winH;
    g_scn.cam.aspect = (float)w / (float)h;
}

/**
 * Renderer::ResizeRenderFbo Resize main render target that will be presented to
 * screen.
 */

void Renderer::ResizeRenderFbo()
{
    //glDeleteFramebuffers(1, &renderFbo);
    //glDeleteTextures(1, &renderTex);
    //CreateRenderPassFbo();
}

/**
 * Renderer::Render Main rendering routine. Update camera, model animations,
 * and execute each render pass.
 *
 * @param hDC Device context handle (for flipping back buffers).
 */

void Renderer::Render()
{
    g_scn.cam.Update();
    float dt = 20.0f * (t2 - t1);
    
    for (auto &model : g_scn.models)
    {
        auto pMesh = g_scn.meshes[model.second.meshName];
        model.second.UpdateAnimation(dt, pMesh);
    }

    for (auto &pass : passes)
    {
        pass.second->Render();
    }

    SwapBuffers(hDC);

    t2 = (float)GetMilliseconds() / 1000.0f;

    if (dt > 150.0)
    {
        t1 = t2;
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

        if (msg.wParam == 0x46)
        {
            g_settings.wireFrame = !g_settings.wireFrame;
        }

        g_scn.cam.HandleKeyDown(msg.wParam);
        break;

    case WM_KEYUP:

        g_scn.cam.HandleKeyUp(msg.wParam);
        break;

    case WM_MOUSEMOVE:

        g_scn.cam.HandleMouseMove(msg.lParam);
        mousePos[0] = (double)GET_X_LPARAM(msg.lParam);
        mousePos[1] = (double)GET_Y_LPARAM(msg.lParam) + 40.0;
        break;

    case WM_LBUTTONDOWN:

        g_scn.cam.HandleMouseDown(msg.lParam);
        DoPick(msg.lParam);
        mouseDown[0] = true;
        mousePos[0] = (double)GET_X_LPARAM(msg.lParam);
        mousePos[1] = (double)GET_Y_LPARAM(msg.lParam) + 40.0;
        break;

    case WM_LBUTTONUP:

        g_scn.cam.HandleMouseUp();
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
    if (passes.count("PickerPass") > 0)
    {
        uint32_t x = GET_X_LPARAM(mouseCoord);
        uint32_t y = g_settings.winH - (GET_Y_LPARAM(mouseCoord) + 40);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, pickerFbo);

        float pixel4[4];
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, &pixel4[0]);
        vec3 pixel = vec3(pixel4[0], pixel4[1], pixel4[2]);

        static bool firstHit = true;

        for (auto &model : g_scn.models)
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
                        g_scn.models[selected].selected = false;
                    }
                }

                selected = model.first;
            }
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

    g_scn.AddDiffTexture("NoiseTex", "", noiseTexID);
}