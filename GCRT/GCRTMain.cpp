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
        "out float exTheta;\n"
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
        "\n"
        "    gl_Position = proj * view * model * vec4(inPos, 1);\n"
        "    exColor = ia * ka + id * theta * kd;\n"
        "    exUV = inUV;\n"
        "    exTheta = theta;\n"
        "}\n"
        ;

    const char* pPSSource =
        "#version 330 core\n"
        "\n"
        "in vec3 exColor;\n"
        "in vec2 exUV;\n"
        "in float exTheta;\n"
        "out vec4 color;\n"
        "\n"
        "uniform sampler2D texture;\n"
        "\n"
        "void main()\n" 
        "{\n"
        "    color = vec4(exColor, 1.0) + 1.5 * exTheta * vec4(texture2D(texture, exUV).rgb, 1);\n"
        "}\n"
        ;

    const char* pVSSourceNM =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec3 inPos;\n"
        "layout(location = 1) in vec3 inNorm;\n"
        "layout(location = 2) in vec2 inUV;\n"
        "layout(location = 3) in vec3 inTan;\n"
        "\n"
        "out vec4 passPos;\n"
        "out vec4 passNorm;\n"
        "out vec4 passTan;\n"
        "out vec4 passBitan;\n"
        "out vec2 passUV;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 proj;\n"
        "uniform mat4 modelInv;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 pos  = model * vec4(inPos, 1);\n"
        "    vec4 norm = modelInv * vec4(inNorm, 1);\n"
        "    vec4 tan  = model * vec4(inTan, 1);\n"
        "\n"
        "    vec3 bitan3 = cross(inTan, inNorm);\n"
        "    vec4 bitan  = modelInv * vec4(bitan3, 1);\n"
        "\n"
        "    gl_Position = proj * view * model * vec4(inPos, 1);\n"
        "\n"
        "    passPos    = pos;\n"
        "    passNorm   = norm;\n"
        "    passTan    = tan;\n"
        "    passBitan  = bitan;\n"
        "    passUV     = inUV;\n"
        "}\n"
        ;

    const char* pPSSourceNM =
        "#version 330 core\n"
        "\n"
        "in vec4 passPos;\n"
        "in vec4 passNorm;\n"
        "in vec4 passTan;\n"
        "in vec4 passBitan;\n"
        "in vec2 passUV;\n"
        "\n"
        "out vec4 color;\n"
        "\n"
        "uniform vec3 lightPos;\n"
        "uniform mat4 modelInv;\n"
        "uniform vec3 ka;"
        "uniform vec3 kd;"
        "uniform float ia;"
        "uniform float id;"
        "\n"
        "uniform sampler2D diffuseTex;\n"
        "uniform sampler2D normalTex;\n"
        "\n"
        "void main()\n"
        "{\n"
        "\n"
        "    mat3 tbn = mat3(passTan.xyz, passBitan.xyz, passNorm.xyz);\n"
        "    vec3 nmNorm = tbn * texture2D(normalTex, passUV).rgb;\n"
        "    vec4 norm = modelInv * vec4(nmNorm, 1);\n"
        "\n"
        "    vec3 lightVec = normalize(lightPos - passPos.xyz);\n"
        "    float dist = length(lightVec);\n"
        "    float theta = max(dot(norm.xyz, lightVec), 0) / (dist * dist);\n"
        "\n"
        "    color = (ia + theta) * vec4(texture2D(diffuseTex, passUV).rgb, 1);\n"
        "}\n"
        ;

    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsID, 1, &pVSSourceNM, NULL);
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
    glShaderSource(psID, 1, &pPSSourceNM, NULL);
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
        vec3(15.0, 15.0, 15.0),
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        4.0f / 3.0f,
        45.0f,
        0.1f,
        100.f
    );

    uint32_t numModels = 1;
    scn.models.resize(numModels);

    scn.materials.resize(2);

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
    pln.Scale(vec3(10.0, 10.0, 1.0));

    scn.models[0].pGeom = make_unique<Plane>(pln);
    scn.models[0].SetMaterial(scn.materials[1]);

    /*Box box;
    box.Create();
    box.Scale(vec3(2.0, 2.0, 2.0));
    box.Translate(vec3(5.0, 5.0, 2.0));

    scn.models[1].pGeom = make_unique<Box>(box);
    scn.models[1].SetMaterial(scn.materials[0]);

    Sphere sph;
    sph.Create(25, 25);
    sph.Scale(vec3(2.0, 2.0, 2.0));
    sph.Translate(vec3(5.0, -5.0, 3.0));

    scn.models[2].pGeom = make_unique<Sphere>(sph);
    scn.models[2].SetMaterial(scn.materials[0]);

    Cylinder cyl;
    cyl.Create(20);
    cyl.Scale(vec3(2.0, 2.0, 2.0));
    cyl.Translate(vec3(-5.0, -5.0, 3.0));

    scn.models[3].pGeom = make_unique<Cylinder>(cyl);
    scn.models[3].SetMaterial(scn.materials[0]);*/
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

    GLuint viewID = glGetUniformLocation(scn.programID, "view");
    glUniformMatrix4fv(viewID, 1, false, &view[0][0]);

    GLuint projID = glGetUniformLocation(scn.programID, "proj");
    glUniformMatrix4fv(projID, 1, false, &proj[0][0]);

    // Lighting parameters.

    vec3 lightPos(10.0f * cosf(t), 10.0f * sinf(t), 5.0f);

    GLuint lightPosID = glGetUniformLocation(scn.programID, "lightPos");
    glUniform3fv(lightPosID, 1, &lightPos[0]);

    float ia = 0.3f;
    float id = 0.5f;
    
    GLuint iaID = glGetUniformLocation(scn.programID, "ia");
    glUniform1fv(iaID, 1, &ia);

    GLuint idID = glGetUniformLocation(scn.programID, "id");
    glUniform1fv(idID, 1, &id);

    for (uint32_t i = 0; i < scn.models.size(); i++)
    {
        scn.models[i].SetShaderParams(scn.programID);
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