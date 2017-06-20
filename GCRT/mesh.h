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

    vec3 pos;
    vec3 dims;
    vec3 angles;

    mat4 scl;
    mat4 trans;
    mat4 rot;

    mat4 model;
    mat4 modelInv;

    virtual void Draw() = 0;
    virtual void UpdateAnimation(float t) {};
    virtual void SetBoneMatrices(GLuint renderProgram) {};

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

    void InitModelMatrices();
    void Translate(vec3 tx);
    void Scale(vec3 dimsIn);
    void Rotate(vec3 thetas);
};