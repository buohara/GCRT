#pragma once

// STD/STL headers

#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <sstream>
#include <memory>
#include <fstream>
#include <map>
#include <chrono>
#include <mutex>

// Windows headers

#include <Windows.h>
#include <Windowsx.h>

// GLM headers

#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/quaternion.hpp"
#include <glm/gtc/type_ptr.hpp>

// DevIL libraries

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

enum RTSettingType
{
    GCRT_UINT,
    GCRT_BOOL,
    GCRT_STRING
};

struct RTCLArg
{
    std::string clName;
    std::string desc;
    std::string defaultVal;
    RTSettingType type;
    uint32_t settingsOffset;
};

struct RTRenderSettings
{
    uint32_t imageW;
    uint32_t imageH;
    uint32_t vLightSets;
    uint32_t vLightSetSize;
    uint32_t camPathDepth;
    uint32_t lightPathDepth;
    uint32_t pixelSamples;
    uint32_t filterSize;
    uint32_t dofSamples;
    uint32_t numThreads;
    uint32_t xBlocks;
    uint32_t yBlocks;
    bool scnFromFile;
    std::string scnFilePath;
    uint32_t numBSDFSamples;
    uint32_t numLightSamples;
};