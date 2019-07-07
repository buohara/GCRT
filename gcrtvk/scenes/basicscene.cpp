#include "scenes.h"

extern RenderSettingsVK g_settings;

void LaunchBasicScene(HINSTANCE hInstance)
{
    g_settings.winW = 1920;
    g_settings.winH = 1080;

    RendererVK rndr(hInstance);

    RenderPassVk phongPass(
        rndr.logicalDevice,
        rndr.deviceMemoryProperties,
        rndr.graphicsQueueIdx,
        true,
        rndr.colorFormat,
        rndr.depthFormat,
        rndr.scSize,
        rndr.scImageViews
    );
    
    rndr.Add(phongPass);

    SceneVk scn(rndr.logicalDevice, rndr.graphicsQueueIdx);
    MeshVk tri(rndr.logicalDevice, rndr.deviceMemoryProperties, TRIANGLE);
    scn.Add(tri);

    scn.RecordUploadScene(rndr.logicalDevice, rndr.deviceMemoryProperties);

    rndr.Upload(scn);

    while (1)
    {
        MSG msg = { 0 };

        while (PeekMessage(&msg, rndr.hWnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        rndr.Render(scn);
    }
}