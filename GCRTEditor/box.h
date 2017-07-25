#pragma once

#include "mesh.h"

struct Box : Mesh
{
    uint32_t numVerts;
    void Create();
    void Draw();

    void GenPositions(vector<vec3> &pos);
    void GenNormals(vector<vec3> &norms);
    void GenUVs(vector<vec2> &uvs);
    void GenTans(vector<vec3> &uvs);
};