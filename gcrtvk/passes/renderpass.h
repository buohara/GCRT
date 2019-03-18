#pragma once

#include "gcrtvk.h"
#include "utilsvk.h"

struct RenderPassVk
{
    bool renderToFrameBuffer = false;
	virtual void Render() = 0;
    virtual VkRenderPass GetRenderPass() = 0;
};