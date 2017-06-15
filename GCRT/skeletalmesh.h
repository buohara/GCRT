#pragma once

#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct BoneTreeNode
{
    string name;
    mat4 boneOffset;
    vector<shared_ptr<BoneTreeNode>> children;
};

struct SkeletalMesh : Mesh
{
    map<string, uint32_t> boneMap;
    BoneTreeNode root;

    void Create(string file);
    void LoadVertexAndBoneData(const aiScene &scene);
    void CreateBoneHierarchy(aiNode &aiNode, BoneTreeNode &btNode);

    void Draw();
};