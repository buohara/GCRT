#pragma once

#include "gcrtcommon.h"

using namespace glm;

enum LightType
{
    POINTLIGHT,
    DIRECTIONAL
};

struct Light
{
    Light(LightType type, vec3 val) : type(type)
    {
        if (type == POINTLIGHT) pos = val;
        if (type == DIRECTIONAL) dir = val;
    }

    LightType type;
    vec3 pos;
    vec3 dir;
};