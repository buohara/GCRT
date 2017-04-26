#pragma once

#include "geometry.h"

struct Sphere : Geometry
{
    uint32_t numSideVerts = 0;
    uint32_t numCapVerts = 0;
    uint32_t topOffset = 0;
    uint32_t bottomOffset = 0;

    void Create(
        uint32_t numSectors, 
        uint32_t numRings,
        GLuint progID
    );

    void Draw();
};