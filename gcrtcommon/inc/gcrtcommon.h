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
#include <filesystem>
#include <assert.h>

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

enum GCRT_RESULT
{
	GCRT_OK
};