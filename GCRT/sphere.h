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
        uint32_t numRings
    );

    void Draw();

    void GenPositions(vector<vec3> &pos, uint32_t numSectors, uint32_t numRings);
    void GenNormals(vector<vec3> &norms, uint32_t numSectors, uint32_t numRings);
    void GenUVs(vector<vec2> &uvs, uint32_t numSectors, uint32_t numRings);
};