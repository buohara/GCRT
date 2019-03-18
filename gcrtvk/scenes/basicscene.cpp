#include "basicscene.h"

extern RenderSettingsVK g_settings;

void BasicScene::LaunchBasicScene(HINSTANCE hInstance)
{
    g_settings.winW = 1920;
    g_settings.winH = 1080;

    BasicScene scn;
    scn.Init(hInstance);
    scn.Render();
}

void BasicScene::Init(HINSTANCE hInstance)
{
    rndr.Init();
    rndr.CreateRenderWindow(hInstance);
    rndr.GetPresentSurface(hInstance);
    rndr.CreateSwapChain();
    rndr.CreateCommandBuffers();
    rndr.CreateFenceObjects();
    rndr.CreateDepth();
    rndr.SetupFrameBuffer();
}

void BasicScene::Render()
{
    while (true)
    {
        rndr.Render();
    }
}

void BasicScene::LoadScene()
{

}