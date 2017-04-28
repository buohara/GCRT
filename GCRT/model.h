#pragma once

#include "geometry.h"
#include "material.h"

struct Model
{
    Geometry geom;
    Material mat;

    void SetShaderParams(GLuint progID);
    void Draw();
};