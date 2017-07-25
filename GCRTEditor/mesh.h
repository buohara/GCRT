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
    GLuint boneIDVboID;
    GLuint boneWtVboID;

    uint32_t numIdcs;
    uint32_t numVerts;
};

struct Mesh
{
    vector<SubMesh> subMeshes;
    string name;
    bool loadFromFile;
    bool animated;
    string filePath;

    virtual void Draw() = 0;
    
    virtual void GetAnimation(float t, mat4 rootTrans, vector<mat4> &bones) 
    {
        if (bones.size() != 1)
        {
            bones.resize(1);
        }

        bones[0] = rootTrans;
    };

    void InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos,
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<ivec4> &boneIDs,
        vector<vec4> &boneWeights
    );

    void InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos, 
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<vec3> &tans,
        vector<ivec4> &boneIDs,
        vector<vec4> &boneWeights
    );

    void Mesh::InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos,
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<uint32_t> &idcs,
        vector<ivec4> &boneIDs,
        vector<vec4> &boneWeights
    );    
};