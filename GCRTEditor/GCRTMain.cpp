#define GLEW_STATIC

#include "renderer.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool resized = false;
uint32_t windowWidth = 1920;
uint32_t windowHeight = 1080;

/**
 * CreateGLContext Create OpenGL context and swapchain.
 * @param  hWnd Window handle for this app.
 * @return Handle to GL context.   
 */

HDC CreateGLContext(HWND hWnd)
{
    // Swapchain format format.

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
 * [WinMain description]
 * @param  hInstance     Process Instance
 * @param  hPrevInstance Previous application instance.
 * @param  lpCmdLine     Command line parameters.
 * @param  nShowCmd      How to show the window (maximized, minimized, ...).
 * @return               Error code.
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
        windowWidth, 
        windowHeight, 
        0, 
        0, 
        hInstance, 
        0
    );

    // Initialize its rendering context.

    HDC hDC = CreateGLContext(hMainWnd);
    Renderer rndr;
    rndr.Init();
    ImGuiGCRTInit(hMainWnd, windowWidth, windowHeight);

    // Main messaging loop.

    while (true)
    {
        rndr.Render(hDC);

        // Check if WinProc resized the window and handle. 

        if (resized)
        {
            RECT rect;
            GetWindowRect(hMainWnd, &rect);
            windowWidth = rect.right - rect.left;
            windowHeight = rect.bottom - rect.top;
            rndr.UpdateViewPorts(windowWidth, windowHeight);
            resized = false;
        }

        while (PeekMessage(&msg, hMainWnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            rndr.HandleInputs(msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

/**
 * WndProc Windows message handler.
 * @param  hWnd    Application window handle.
 * @param  message Message to process.
 * @param  wParam  Message parameters.
 * @param  lParam  More message parameters.
 * @return         Status code for message handler.
 */

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:

        resized = true;
        break;

    case WM_CLOSE:
        exit(0);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}