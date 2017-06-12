#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

struct SubMesh
{
    GLuint vaoID;
    GLuint posVboID;
    GLuint normVboID;
    GLuint uvVboID;
    GLuint tanVboID;
    GLuint idxVboID;
    uint32_t numIdcs;
    uint32_t numVerts;
};

struct Mesh
{
    vector<SubMesh> subMeshes;
    string name;
    bool blenderModel;
    string blenderPath;

    virtual void Draw() = 0;

    void InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos,
        vector<vec3> &norms,
        vector<vec2> &uvs
    );

    void InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos, 
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<vec3> &tans
    );

    void InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos,
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<uint32_t> &idcs
    );
};