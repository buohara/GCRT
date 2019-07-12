#pragma once

#include "renderer.h"
#include "input.h"
#include "mainpass.h"
#include "depthpass.h"

struct AnimationScene
{
    Renderer rndr;
    Scene scn;
    static void LaunchAnimationScene(HINSTANCE hInstance);
    AnimationScene(HINSTANCE hInstance);

private:

    void Render();
    void LoadScene();
};