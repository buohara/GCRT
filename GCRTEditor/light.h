#pragma once

#include "GCRT.h"

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