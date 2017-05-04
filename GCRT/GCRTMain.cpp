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
    glClearColor(0.1f, 0.1f, 0.1f, 0.5f);

    scn.shaders.resize(1);
    scn.shaders[0].Create(
        string("NormalShader"),
        string("NormalShader.vs"),
        string("NormalShader.fs")
    );

    glUseProgram(scn.shaders[0].program);

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

    uint32_t numModels = 4;
    scn.models.resize(numModels);

    uint32_t numMaterials = 2;
    scn.materials.resize(numMaterials);

    scn.materials[0].name = "Dirt";
    scn.materials[0].LoadDiffuseTexture(string("E:/drive/GCRT/asset/dirtdiffuse.jpg"));
    scn.materials[0].LoadNormalTexture(string("E:/drive/GCRT/asset/dirtnormal.jpg"));
    scn.materials[0].SetDiffuse(vec3(0.5, 0.5, 0.5));
    scn.materials[0].SetAmbient(vec3(0.5, 0.5, 0.5));

    scn.materials[1].name = "Grass";
    scn.materials[1].LoadDiffuseTexture(string("E:/drive/GCRT/asset/grassdiffuse.jpg"));
    scn.materials[1].LoadNormalTexture(string("E:/drive/GCRT/asset/grassnormal.jpg"));
    scn.materials[1].SetDiffuse(vec3(0.5, 0.5, 0.5));
    scn.materials[1].SetAmbient(vec3(0.5, 0.5, 0.5));

    // Plane

    Plane pln;
    pln.Create(10, 10);
    pln.Scale(vec3(20.0, 20.0, 1.0));

    scn.models[0].pGeom = make_unique<Plane>(pln);
    scn.models[0].SetMaterial(scn.materials[1]);

    Box box;
    box.Create();
    box.Scale(vec3(1.0, 1.0, 1.0));
    box.Translate(vec3(-5.0, 5.0, 1.0));

    scn.models[1].pGeom = make_unique<Box>(box);
    scn.models[1].SetMaterial(scn.materials[0]);

    Sphere sph;
    sph.Create(25, 25);
    sph.Scale(vec3(5.0, 5.0, 5.0));
    sph.Translate(vec3(0.0, 0.0, 3.0));

    scn.models[2].pGeom = make_unique<Sphere>(sph);
    scn.models[2].SetMaterial(scn.materials[0]);

    Cylinder cyl;
    cyl.Create(20);
    cyl.Scale(vec3(2.0, 2.0, 2.0));
    cyl.Translate(vec3(-5.0, -5.0, 3.0));

    scn.models[3].pGeom = make_unique<Cylinder>(cyl);
    scn.models[3].SetMaterial(scn.materials[0]);
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
    mat4 proj = scn.cam.GetProjection();
    mat4 view = scn.cam.GetView();
    GLuint program = scn.shaders[0].program;

    GLuint viewID = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(viewID, 1, false, &view[0][0]);

    GLuint projID = glGetUniformLocation(program, "proj");
    glUniformMatrix4fv(projID, 1, false, &proj[0][0]);

    // Lighting parameters.

    vec3 lightPos(20.0f * cosf(t), 20.0 * sinf(t), 5.0f);

    GLuint lightPosID = glGetUniformLocation(program, "lightPos");
    glUniform3fv(lightPosID, 1, &lightPos[0]);

    vec3 camPos = scn.cam.pos;

    GLuint camPosID = glGetUniformLocation(program, "camPos");
    glUniform3fv(camPosID, 1, &camPos[0]);

    float ia = 0.1f;
    float id = 0.5f;
    
    GLuint iaID = glGetUniformLocation(program, "ia");
    glUniform1fv(iaID, 1, &ia);

    GLuint idID = glGetUniformLocation(program, "id");
    glUniform1fv(idID, 1, &id);

    for (uint32_t i = 0; i < scn.models.size(); i++)
    {
        scn.models[i].SetShaderParams(program);
        scn.models[i].Draw();
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