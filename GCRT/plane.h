#pragma once

#include "geometry.h"

struct Plane : Geometry
{
    uint32_t numVerts;
    void Create(uint32_t rows, uint32_t cols);
    void Draw();

    void GenPositions(vector<vec3> &pos, uint32_t rows, uint32_t cols);
    void GenNormals(vector<vec3> &norms, uint32_t rows, uint32_t cols);
    void GenUVs(vector<vec2> &uvs, uint32_t rows, uint32_t cols);
    void GenTangents(vector<vec3> &tans, uint32_t rows, uint32_t cols);
};