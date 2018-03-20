#pragma once

#include "renderer.h"
#include "input.h"
#include "mainpass.h"

struct TerrainScene
{
    Renderer rndr;
    static void LaunchTerrainScene(HINSTANCE hInstance);
    void Init(HINSTANCE hInstance);
    void Render();
};