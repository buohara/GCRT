#include "basicscene.h"

extern RenderSettingsVK g_settings;

void LaunchBasicScene(HINSTANCE hInstance)
{
    g_settings.winW = 1920;
    g_settings.winH = 1080;

    RendererVK rndr;
    SceneVk scn;

    BasicScene scn;
    MeshVk tri(TRIANGLE);

    scn.Init(hInstance);
    scn.Render();
}