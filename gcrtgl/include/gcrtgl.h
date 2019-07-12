#pragma once

#include "gcrtcommon.h"
#include "utilscommon.h"

#define GLEW_STATIC

#include <glew.h>
#include <wglew.h>
#include <gl/GL.h>

struct RenderSettings
{
    uint32_t winW;
    uint32_t winH;
    bool wireFrame;
    bool useDOF;
    bool useBloom;
    bool useSkyBox;
    uint32_t msaaSamples;
    bool loadSceneFromFile;
    std::string sceneName;
};