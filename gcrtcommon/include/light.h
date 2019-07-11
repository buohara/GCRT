#pragma once

#include "gcrtcommon.h"

using namespace glm;

struct DirectionalLight
{
    vec3 pos;
    vec3 look;
};

struct PointLight
{
    vec3 pos;
};