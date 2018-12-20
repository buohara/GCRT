#pragma once

// STD/STL headers

#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <sstream>
#include <memory>
#include <fstream>
#include <map>
#include <chrono>

// GL headers

#define GLEW_STATIC

#include <glew.h>
#include <wglew.h>
#include <gl/GL.h>

// Windows headers

#include <Windows.h>
#include <Windowsx.h>
#include <shellapi.h>

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