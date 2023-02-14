#pragma once

#if 0

#include "mesh.h"
#include "noise.h"

struct PerlinTerrain : Mesh
{
    uint32_t numVerts;
    float spacing;
    void Create(uint32_t ndiv);
    void Draw();

    void GenPositions(vector<vec3> &pos);
    void GenNormals(vector<vec3> &norms);
    void GenUVs(vector<vec2> &uvs);
    void GenTans(vector<vec3> &uvs);
};

#endif