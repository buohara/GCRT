#pragma once

#include "renderer.h"
#include "input.h"
#include "mainpass.h"
#include "depthpass.h"

struct AnimationScene
{
    Renderer rndr;
    static void LaunchAnimationScene(HINSTANCE hInstance);
    void Init(HINSTANCE hInstance);
    void Render();
};