#include "scenes.h"

extern RenderSettingsVK g_settings;

void LaunchBasicScene(HINSTANCE hInstance)
{
    g_settings.winW = 1920;
    g_settings.winH = 1080;

    RendererVK rndr(hInstance);

    SceneVk scn(rndr.logicalDevice, rndr.graphicsQueueIdx);
    MeshVk tri(rndr.logicalDevice, rndr.deviceMemoryProperties, TRIANGLE);
    scn.Add(tri);

    scn.RecordUploadScene(rndr.logicalDevice, rndr.deviceMemoryProperties);

    rndr.Upload(scn);

    while (1) rndr.Render(scn);
}