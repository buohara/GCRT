#pragma once

#include "gcrt.h"

uint32_t FindProperties(
    uint32_t memoryTypeBitsRequirement,
    VkMemoryPropertyFlags requiredProperties,
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties
);