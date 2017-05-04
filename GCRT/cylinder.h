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

    void GenPositions(vector<vec3> &pos, uint32_t numSectors);
    void GenNormals(vector<vec3> &norms, uint32_t numSectors);
    void GenUVs(vector<vec2> &uvs, uint32_t numSectors);
    void GenTans(vector<vec3> &tans, uint32_t numSectors);
};