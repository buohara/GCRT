#pragma once

#include "plane.h"
#include "material.h"
#include "camera.h"
#include "light.h"

struct Model
{
    string meshName;
    string matName;
    vec3 pickerColor;
    bool selected;
};