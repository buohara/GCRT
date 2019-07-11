#pragma once

#include "gcrtcommon.h"
#include "shader.h"
#include "scene.h"

struct RenderPass
{
    virtual void Render() = 0;
};