#pragma once

#include "mesh.h"

struct Sphere : Mesh
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
    void GenTans(vector<vec3> &tans, uint32_t numSectors, uint32_t numRings);

    float Intersect(vec3 org, vec3 ray);
};