#include "animationscene.h"

extern RenderSettings g_settings;
extern bool gResized;

/**
 * AnimationScene::LaunchAnimationScene Create scene instance and launch it.
 * @param hInstance Process handle. Need to pass to renderer for creating GL context.
 */

void AnimationScene::LaunchAnimationScene(HINSTANCE hInstance)
{
    AnimationScene animScn;
    animScn.Init(hInstance);
    animScn.Render();
}

/**
 * AnimationScene::Init Initialize scene's renderer.
 * @param hInstance Process handle to create GL context.
 */

void AnimationScene::Init(HINSTANCE hInstance)
{
    g_settings.loadSceneFromFile    = false;
    g_settings.msaaSamples          = 4;
    g_settings.useBloom             = false;
    g_settings.useDOF               = false;
    g_settings.winH                 = 1080;
    g_settings.winW                 = 1920;
    g_settings.wireFrame            = false;

    rndr.CreateRenderWindow(
        hInstance,
        "AnimationScene"
    );

    rndr.CreateGLContext();

    rndr.Init();

    DepthPass depthPass;
    depthPass.Init();

    MainPass mainPass;
    mainPass.Init(depthPass.depthTexOut, false, true);

    rndr.passes["DepthPass"]    = make_shared<DepthPass>(depthPass);
    rndr.passes["MainPass"]     = make_shared<MainPass>(mainPass);
}

/**
 * AnimationScene::Render Scene's main rendering loop.
 */

void AnimationScene::Render()
{
    MSG msg = { 0 };

    while (true)
    {
        rndr.Render();

        if (gResized)
        {
            RECT rect;
            GetWindowRect(rndr.hWnd, &rect);
            
            g_settings.winW = rect.right - rect.left;
            g_settings.winH = rect.bottom - rect.top;

            rndr.UpdateViewPorts();
            gResized = false;
        }

        while (PeekMessage(&msg, rndr.hWnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            rndr.HandleInputs(msg);
            DispatchMessage(&msg);
        }
    }
}