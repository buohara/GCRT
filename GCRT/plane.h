#pragma once

#include "geometry.h"

struct Plane : Geometry
{
    uint32_t numVerts;
    void Create(uint32_t rows, uint32_t cols, GLuint progID);
    void Draw();
};