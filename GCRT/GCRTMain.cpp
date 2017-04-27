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
    GLint isCompiled = 0;
    GLchar compileBuf[512];

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    const char* pVSSource =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec3 inPos;\n"
        "layout(location = 1) in vec3 inNorm;\n"
        "layout(location = 2) in vec2 inUV;\n"
        "\n"
        "out vec3 exColor;\n"
        "out vec2 exUV;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 proj;\n"
        "uniform mat4 modelInv;\n"
        "\n"
        "uniform vec3 lightPos;\n"
        "uniform vec3 ka;"
        "uniform vec3 kd;"
        "uniform float ia;"
        "uniform float id;"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 pos = model * vec4(inPos, 1);\n"
        "    vec4 norm = modelInv * vec4(inNorm, 1);\n"
        "\n"
        "    vec3 lightVec = normalize(lightPos - pos.xyz);\n"
        "    float dist = length(lightVec);\n"
        "    float theta = max(dot(norm.xyz, lightVec), 0) / (dist * dist);\n"
        "    gl_Position = proj * view * model * vec4(inPos, 1);\n"
        "    exColor = ia * ka + id * theta * kd;\n"
        "    exUV = inUV;\n"
        "}\n"
        ;

    const char* pPSSource =
        "#version 330 core\n"
        "\n"
        "in vec3 exColor;\n"
        "in vec2 exUV;\n"
        "out vec4 color;\n"
        "\n"
        "uniform sampler2D texture;\n"
        "\n"
        "void main()\n" 
        "{\n"
        "    color = vec4(exColor, 1.0) + vec4(texture2D(texture, exUV), 1);\n"
        "}\n"
        ;

    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsID, 1, &pVSSource, NULL);
    glCompileShader(vsID);

#ifdef _DEBUG
    glGetShaderiv(vsID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &maxLength);
        glGetShaderInfoLog(vsID, maxLength, &maxLength, compileBuf);
        glDeleteShader(vsID);
        printf("VS compile error: %s\n", compileBuf);
        __debugbreak();
    }
#endif

    GLuint psID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(psID, 1, &pPSSource, NULL);
    glCompileShader(psID);

#ifdef _DEBUG
    glGetShaderiv(psID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(psID, GL_INFO_LOG_LENGTH, &maxLength);
        glGetShaderInfoLog(psID, maxLength, &maxLength, compileBuf);
        glDeleteShader(psID);
        printf("PS compile error: %s\n", compileBuf);
        __debugbreak();
    }
#endif

    GLint programID = glCreateProgram();
    glAttachShader(programID, vsID);
    glAttachShader(programID, psID);
    glLinkProgram(programID);
    glUseProgram(programID);

    scn.programID = programID;

    // Create camera

    scn.cam.Init(
        vec3(8.0, 8.0, 8.0),
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        4.0f / 3.0f,
        75.0f,
        0.1f,
        100.f
    );

    // Plane

    Plane pln;
    pln.Create(10, 10, programID);

    scn.m1.geom = pln;
    scn.m1.mat.LoadTexture(wstring(L"E:\drive\GCRT\asset\earthmap1k"));
    scn.m1.mat.SetDiffuse(vec3(0.7, 0.1, 0.1));
    scn.m1.mat.SetAmbient(vec3(0.7, 0.1, 0.1));
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
    mat4 viewInv = inverseTranspose(view);

    GLuint viewID = glGetUniformLocation(scn.programID, "view");
    glUniformMatrix4fv(viewID, 1, false, &view[0][0]);

    GLuint projID = glGetUniformLocation(scn.programID, "proj");
    glUniformMatrix4fv(projID, 1, false, &proj[0][0]);

    // Lighting parameters.

    vec3 lightPos(10.0f * cosf(t), 10.0f * sinf(t), 5.0f + cosf(2 * t));

    GLuint lightPosID = glGetUniformLocation(scn.programID, "lightPos");
    glUniform3fv(lightPosID, 1, &lightPos[0]);

    float ia = 0.3f;
    float id = 0.8f;
    
    GLuint iaID = glGetUniformLocation(scn.programID, "ia");
    glUniform1fv(iaID, 1, &ia);

    GLuint idID = glGetUniformLocation(scn.programID, "id");
    glUniform1fv(idID, 1, &id);

    //scn.box1.Draw();
    //scn.box2.Draw();
    scn.m1.geom.Draw();
    //scn.cyl.Draw();
    //scn.sph.Draw();

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
    wc.lpszClassName    = L"GLWindow";
    wc.style            = CS_OWNDC;
    
    if (!RegisterClass(&wc))
    {
        return 1;
    }

    // Create the main GL Window.

    HWND hMainWnd = CreateWindowW(
        wc.lpszClassName, 
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