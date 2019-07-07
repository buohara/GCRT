#include "scenes.h"

extern RenderSettingsVK g_settings;

void LaunchBasicScene(HINSTANCE hInstance)
{
    g_settings.winW = 1920;
    g_settings.winH = 1080;

    RendererVK rndr(hInstance);
    
    RenderPassVk phongPass;
    phongPass.scSize = 2;
    phongPass.graphicsQueueIdx = rndr.graphicsQueueIdx;
    phongPass.Init(rndr.logicalDevice, true);

    phongPass.frameBuffers.push_back(rndr.frameBuffers[0]);
    phongPass.frameBuffers.push_back(rndr.frameBuffers[1]);

    rndr.Add(phongPass);

    SceneVk scn(rndr.logicalDevice, rndr.graphicsQueueIdx);
    MeshVk tri(rndr.logicalDevice, rndr.deviceMemoryProperties, TRIANGLE);
    scn.Add(tri);

    scn.RecordUploadScene(rndr.logicalDevice, rndr.deviceMemoryProperties);

    rndr.Upload(scn);

    while (1) rndr.Render(scn);
}