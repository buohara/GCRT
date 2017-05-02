#pragma once

#include "geometry.h"

struct Box : Geometry
{
    uint32_t numVerts;
    void Create();
    void Draw();

    void GenPositions(vector<vec3> &pos);
    void GenNormals(vector<vec3> &norms);
    void GenUVs(vector<vec2> &uvs);
};