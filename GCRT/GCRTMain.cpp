#define GLEW_STATIC

#include "scene.h"

// Forward declarations.

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/**
 * Create GLContext -
 *
 * @param
 */

HDC CreateGLContext(HWND hWnd)
{
    // Pixel format.

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = 32;
    pfd.cDepthBits  = 32;
    pfd.iLayerType  = PFD_MAIN_PLANE;

    HDC hDC = GetDC(hWnd);

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

    return hDC;
}

/**
 * InitScene -
 */

void InitScene(Scene &scn)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(1.0f, 1.0f, 1.0f, 0.5f);

    // Create camera

    scn.cam.Init(
        vec3(15.0, 15.0, 15.0),
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        4.0f / 3.0f,
        45.0f,
        0.1f,
        100.f
    );

    // Load Shaders.

    scn.shaders["BasicShader"].Create(
        string("BasicShader"),
        string("BasicShader.vs"),
        string("BasicShader.fs")
    );

    scn.shaders["BumpShader"].Create(
        string("BumpShader"),
        string("BumpShader.vs"),
        string("BumpShader.fs")
    );

    // Load textures.

    scn.textures["DirtDiffuse"] = 
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/dirtdiffuse.jpg"));

    scn.textures["DirtNormal"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/dirtnormal.jpg"));

    scn.textures["GrassDiffuse"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/grassdiffuse.jpg"));

    scn.textures["GrassNormal"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/grassNormal.jpg"));

    // Create materials.

    BumpMaterial dirtMat;
    dirtMat.name = "Dirt";
    dirtMat.diffuseTexID = scn.textures["DirtDiffuse"];
    dirtMat.normalTexID = scn.textures["DirtNormal"];
    dirtMat.program = scn.shaders["BumpShader"].program;
    scn.materials["Dirt"] = make_shared<BumpMaterial>(dirtMat);

    BumpMaterial grassMat;
    dirtMat.name = "Grass";
    dirtMat.diffuseTexID = scn.textures["GrassDiffuse"];
    dirtMat.normalTexID = scn.textures["GrassNormal"];
    dirtMat.program = scn.shaders["BumpShader"].program;
    scn.materials["Grass"] = make_shared<BumpMaterial>(dirtMat);

    BasicMaterial basicBlueMat;
    basicBlueMat.name = "BasicBlue";
    basicBlueMat.program = scn.shaders["BasicShader"].program;
    basicBlueMat.kd = vec3(0.1, 0.1, 0.7);
    scn.materials["BasicBlue"] = make_shared<BasicMaterial>(basicBlueMat);

    // Plane

    Plane pln;
    pln.Create(10, 10);
    pln.Scale(vec3(20.0, 20.0, 1.0));

    scn.models["Plane"].pGeom = make_shared<Plane>(pln);
    scn.models["Plane"].SetMaterial(scn.materials["Grass"]);

    Box box;
    box.Create();
    box.Scale(vec3(1.0, 1.0, 1.0));
    box.Translate(vec3(-5.0, 5.0, 1.0));

    scn.models["Box"].pGeom = make_shared<Box>(box);
    scn.models["Box"].SetMaterial(scn.materials["Dirt"]);

    Sphere sph;
    sph.Create(25, 25);
    sph.Scale(vec3(5.0, 5.0, 5.0));
    sph.Translate(vec3(0.0, 0.0, 3.0));

    scn.models["Sphere"].pGeom = make_shared<Sphere>(sph);
    scn.models["Sphere"].SetMaterial(scn.materials["Dirt"]);

    Cylinder cyl;
    cyl.Create(20);
    cyl.Scale(vec3(2.0, 2.0, 2.0));
    cyl.Translate(vec3(-5.0, -5.0, 3.0));

    scn.models["Cylinder"].pGeom = make_shared<Cylinder>(cyl);
    scn.models["Cylinder"].SetMaterial(scn.materials["BasicBlue"]);
}

/**
 * Draw -
 */

void Draw(HDC hDC, Scene &scn)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float t = 0;

    // Update camera and get projection/view matrices.

    scn.cam.Update();
    GLuint program = scn.shaders["NormalShader"].program;

    // Lighting parameters.

    vec3 lightPos(20.0f * cosf(t), 20.0 * sinf(t), 5.0f);

    map<string, Model>::iterator it;
    for (it = scn.models.begin(); it != scn.models.end(); it++)
    {
        glUseProgram((*it).second.program);
        (*it).second.SetCamera(scn.cam);
        (*it).second.SetLights(lightPos);
        (*it).second.Draw();
    }

    SwapBuffers(hDC);
    t += 0.01f;
}

/**
 * WinMain - 
 */

int CALLBACK WinMain(
    __in HINSTANCE hInstance,
    __in HINSTANCE hPrevInstance, 
    __in LPSTR lpCmdLine,
    __in int nShowCmd
)
{
    MSG msg = { 0 };
    
    WNDCLASS wc         = { 0 };
    wc.lpfnWndProc      = WndProc;
    wc.hInstance        = hInstance;
    wc.hbrBackground    = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName    = "GLWindow";
    wc.style            = CS_OWNDC;
    
    if (!RegisterClass(&wc))
    {
        return 1;
    }

    // Create the main GL Window.

    HWND hMainWnd = CreateWindowW(
        L"GLWindow", 
        L"GLWindow", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
        0, 
        0, 
        1920, 
        1080, 
        0, 
        0, 
        hInstance, 
        0
    );

    // Initialize its rendering context.

    HDC hDC = CreateGLContext(hMainWnd);
    Scene scn;
    InitScene(scn);

    // Main messaging loop.

    while (true)
    {
        Draw(hDC, scn);
        RECT rect;
        GetWindowRect(hMainWnd, &rect);
        scn.cam.aspect = (float)(rect.right - rect.left) / (float)(rect.bottom - rect.top);

        while (PeekMessage(&msg, hMainWnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            scn.HandleInputs(msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

/**
 * WinProc -
 */

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:

        glViewport(
            0,
            0,
            LOWORD(lParam),
            HIWORD(lParam)
        );
        break;

    case WM_CLOSE:
        exit(0);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}