#pragma once

#include "plane.h"
#include "material.h"

struct Model
{
    unique_ptr<Geometry> pGeom;
    Material mat;

    void SetShaderParams(GLuint progID);
    void SetMaterial(const Material &matIn);
    void Draw();
};