#pragma once

#include "renderer.h"
#include "input.h"
#include "mainpass.h"
#include "depthpass.h"

struct EnvMapScene
{
    Renderer rndr;
    Scene scn;

    static void LaunchEnvMapScene(HINSTANCE hInstance);
    EnvMapScene(HINSTANCE hInstance);
    void Render();
    void LoadScene();
};