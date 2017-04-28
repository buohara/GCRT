#pragma once

#include "geometry.h"

struct Box : Geometry
{
    uint32_t numVerts;
    void Create();
    void Draw();
};