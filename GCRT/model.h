#pragma once

#include "plane.h"
#include "material.h"

struct Model
{
    Plane geom;
    Material mat;

    void SetShaderParams(GLuint progID);
    void Draw();
};