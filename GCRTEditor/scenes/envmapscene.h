#pragma once

#include "renderer.h"
#include "input.h"
#include "mainpass.h"
#include "depthpass.h"

struct EnvMapScene
{
	Renderer rndr;
	static void LaunchEnvMapScene(HINSTANCE hInstance);
	void Init(HINSTANCE hInstance);
	void Render();
    void LoadScene();
};