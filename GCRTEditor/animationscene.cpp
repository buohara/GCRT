#include "animationscene.h"

bool gResized;
uint32_t gWindowHeight;
uint32_t gWindowWidth;

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
    rndr.CreateRenderWindow(
        hInstance,
        1920,
        1080,
        "AnimationScene"
    );

    rndr.CreateGLContext();

    rndr.settings.loadSceneFromFile = false;
    rndr.settings.msaaSamples       = 4;
    rndr.settings.useBloom          = false;
    rndr.settings.useDOF            = false;
    rndr.settings.winH              = 1080;
    rndr.settings.winW              = 1920;
    rndr.settings.wireFrame         = false;

    rndr.Init();
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
            gWindowWidth = rect.right - rect.left;
            gWindowHeight = rect.bottom - rect.top;

            rndr.UpdateViewPorts(gWindowWidth, gWindowHeight);
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