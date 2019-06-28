#pragma once

#include "gcrtvk.h"

using namespace std;

void CHECK_RESULT(VkResult res);
VkShaderModule LoadShader(VkDevice &logicalDevice, string file);

uint32_t FindProperties(
    uint32_t memoryTypeBitsRequirement,
    VkMemoryPropertyFlags requiredProperties,
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties
);