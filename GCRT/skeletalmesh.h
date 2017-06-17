#pragma once

#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "animation.h"

struct BoneTreeNode
{
    string name;
    vector<shared_ptr<BoneTreeNode>> children;
    Animation animation;
    mat4 boneOffset;

    void GetBoneMatrices(
        float t, 
        vector<mat4> &matrices,
        mat4 parent,
        map<string, uint32_t> &boneMap
    );

    bool LoadAnimation(aiNodeAnim &anim);
};

struct SkeletalMesh : Mesh
{
    map<string, uint32_t> boneMap;
    BoneTreeNode root;

    void Create(string file);

    void LoadVertexAndBoneData(
        const aiScene &scene,
        map<string, mat4> boneOffsets
    );
    
    void CreateBoneHierarchy(
        aiNode &aiNode, 
        BoneTreeNode &btNode,
        map<string, mat4> boneOffsets
    );
    
    void LoadVertexData(
        aiMesh &mesh,
        vector<vec3> &pos,
        vector<vec3> &norm,
        vector<vec2> &UV,
        vector<uint32_t> &idcs
    );

    void LoadBoneData(
        aiMesh &mesh,
        vector<uvec4> &boneIDs,
        vector<vec4> &boneWts,
        map<string, mat4> boneOffsets
    );

    void LoadAnimations(const aiScene &scene);
    void SetBoneMatrices(float t, GLuint renderProgram);

    void Draw();
};