#pragma once

#include "gcrtcommon.h"

// DevIL libraries

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

// Vulkan

#include "vulkan.h"
#include "vulkan_win32.h"

struct RenderSettingsVK
{
    uint32_t winW;
    uint32_t winH;
};