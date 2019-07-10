#pragma once

#include "animation.h"
#include "gcrt.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct aiNode;
struct aiScene;
struct aiNodeAnim;
struct aiMesh;

using namespace std;
using namespace glm;

struct BoneTreeNode
{
    string name;
    vector<shared_ptr<BoneTreeNode>> children;
    Animation animation;
    mat4 boneOffset;
    mat4 parentOffset;

    void GetBoneMatrices(
        float t, 
        vector<mat4> &matrices,
        mat4 parent,
        map<string, uint32_t> &boneMap
    );

    bool LoadAnimation(aiNodeAnim &anim);
};

void CreateBoneHierarchy(
    aiNode& aiNode,
    BoneTreeNode& btNode,
    map<string, mat4>& boneOffsets
);

void LoadVertexData(
    aiMesh& mesh,
    vector<vec3>& pos,
    vector<vec3>& norm,
    vector<vec2>& UV,
    vector<uint32_t>& idcs
);

void LoadBoneData(
    aiMesh& mesh,
    vector<ivec4>& boneIDs,
    vector<vec4>& boneWts,
    map<string, mat4>& boneOffsets
);

mat4 aiMatrix4x4ToGlm(aiMatrix4x4& from);

void LoadMaterials(const aiScene& scene);
void LoadAnimations(const aiScene& scene);
void GetAnimation(float t, mat4 rootTrans, vector<mat4>& bones);