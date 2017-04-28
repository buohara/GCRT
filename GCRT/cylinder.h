#pragma once

#include "geometry.h"

struct Cylinder : Geometry
{
    uint32_t numSideVerts = 0;
    uint32_t numCapVerts = 0;
    uint32_t topOffset = 0;
    uint32_t bottomOffset = 0;

    void Create(uint32_t numSectors);

    void Draw();
};