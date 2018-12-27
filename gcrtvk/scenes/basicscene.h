#pragma once

#include "renderervk.h"

struct BasicScene
{
    RendererVK rndr;
    static void LaunchBasicScene(HINSTANCE hInstance);
    void Init(HINSTANCE hInstance);
    void Render();
    void LoadScene();
};