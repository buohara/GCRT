#pragma once

#include "gcrtvk.h"

using namespace std;

void CHECK_RESULT(VkResult res);
VkShaderModule LoadShader(VkDevice &logicalDevice, string file);