#pragma once

#include "GCRT.h"
#include "plane.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "skeletalmesh.h"
#include "utils.h"

using namespace std;
using namespace glm;

enum MeshType
{
    BOX,
    PLANE,
    SPHERE,
    CYLINDER,
    SKELETAL
};

struct SubMesh
{
    uint32_t numIdcs;
    uint32_t numVerts;

    GLuint vaoID;
    
    GLuint posVboID;
    GLuint normVboID;
    GLuint uvVboID;
    GLuint tanVboID;
    GLuint idxVboID;
    GLuint boneIDVboID;
    GLuint boneWtVboID;
};

struct MeshGL
{
    vector<SubMesh> subMeshes;
    string name;
    bool loadFromFile;
    bool animated;
    string filePath;
    bool invert = false;
    MeshType type;

    // How many vertices to draw for simple shapes.

    uint32_t numVerts;
    
    // How many vertices to draw for different parts of a sphere/cylinder.

    uint32_t numSideVerts   = 0;
    uint32_t numCapVerts    = 0;
    uint32_t topOffset      = 0;
    uint32_t bottomOffset   = 0;

    // Skeletal mesh data structures

    map<string, uint32_t> boneMap;
    BoneTreeNode root;
    mat4 globalInverse;
    static const uint32_t maxBones = 64;

    void LoadVertexAndBoneData(
        const aiScene& scene,
        map<string, mat4>& boneOffsets
    );

    MeshGL(MeshType type, uint32_t rows, uint32_t cols);
    MeshGL(MeshType type, uint32_t numSectors);
    MeshGL(MeshType type, string file);
    MeshGL(MeshType type);
    MeshGL(MeshType type, uint32_t numSectors, uint32_t numRings, bool invertIn);

    void Draw();
    
    void GetAnimation(float t, mat4 rootTrans, vector<mat4> &bones) 
    {
        if (bones.size() != 1) bones.resize(1);
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

    void InitVertexObjects(
        uint32_t subMeshIdx,
        vector<vec3> &pos,
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<uint32_t> &idcs,
        vector<ivec4> &boneIDs,
        vector<vec4> &boneWeights
    );
};