#include "skeletalmesh.h"

/**
 * Create
 */

void SkeletalMesh::Create(string file)
{
    Assimp::Importer importer;
    
    const aiScene scene = *(importer.ReadFile(
        file, 
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
    ));
    
    subMeshes.resize(scene.mNumMeshes);
    animated = (scene.mNumAnimations > 0);

    LoadVertexAndBoneData(scene);

    aiNode &scnRoot = *(scene.mRootNode);
    root.name = scnRoot.mName.C_Str();
    CreateBoneHierarchy(scnRoot, root);
}

/**
 * CreateBoneHierarchy -
 */

void SkeletalMesh::CreateBoneHierarchy(aiNode &aiNode, BoneTreeNode &btNode)
{
    for (uint32_t i = 0; i < aiNode.mNumChildren; i++)
    {
        BoneTreeNode child;
        child.name = aiNode.mChildren[i]->mName.C_Str();
        memcpy(&child.boneOffset[0], &aiNode.mChildren[i]->mTransformation, 16 * sizeof(float));

        btNode.children.push_back(make_shared<BoneTreeNode>(child));
        CreateBoneHierarchy(*aiNode.mChildren[i], *btNode.children[i]);
    }
}

/**
 * LoadVertexAndBoneData - 
 */

void SkeletalMesh::LoadVertexAndBoneData(const aiScene &scene)
{
    for (uint32 i = 0; i < scene.mNumMeshes; i++)
    {
        aiMesh &mesh = *(scene.mMeshes[i]);

        vector<vec3> pos;
        vector<vec3> norm;
        vector<vec2> uv;
        vector<uint32_t> idcs;

        subMeshes[i].numVerts = mesh.mNumVertices;
        subMeshes[i].numIdcs = 3 * mesh.mNumFaces;

        pos.resize(subMeshes[i].numVerts);
        norm.resize(subMeshes[i].numVerts);
        uv.resize(subMeshes[i].numVerts);
        idcs.resize(subMeshes[i].numIdcs);

        // Vertex attributes.

        for (uint32_t j = 0; j < subMeshes[i].numVerts; j++)
        {
            pos[j].x = mesh.mVertices[j].x;
            pos[j].y = mesh.mVertices[j].y;
            pos[j].z = mesh.mVertices[j].z;

            norm[j].x = mesh.mNormals[j].x;
            norm[j].y = mesh.mNormals[j].y;
            norm[j].z = mesh.mNormals[j].z;

            if (scene.mNumTextures > 0)
            {
                uv[j].x = mesh.mTextureCoords[0][j].x;
                uv[j].y = 1.0f - mesh.mTextureCoords[0][j].y;
            }
        }

        // Triangle indices.

        for (uint32_t j = 0; j < mesh.mNumFaces; j++)
        {
            idcs[3 * j] = mesh.mFaces[j].mIndices[0];
            idcs[3 * j + 1] = mesh.mFaces[j].mIndices[1];
            idcs[3 * j + 2] = mesh.mFaces[j].mIndices[2];
        }

        if (animated == false)
        {
            InitVertexObjects(i, pos, norm, uv, idcs);
            continue;
        }

        // Bone IDs and weights.

        vector<uvec4> boneIDs;
        vector<vec4> boneWeights;
        vector<uint32_t> boneScratch(subMeshes[i].numVerts, 0);

        boneIDs.resize(subMeshes[i].numVerts);
        boneWeights.resize(subMeshes[i].numVerts);

        for (uint32_t j = 0; j < mesh.mNumBones; j++)
        {
            aiBone &bone = *(mesh.mBones[j]);
            boneMap[bone.mName.C_Str()] = j;

            for (uint32_t k = 0; k < mesh.mBones[j]->mNumWeights; k++)
            {
                uint32_t vertID = bone.mWeights[k].mVertexId;
                float weight = bone.mWeights[k].mWeight;

                if (boneScratch[vertID] == 0)
                {
                    boneIDs[vertID].x = j;
                    boneWeights[vertID].x = weight;
                    boneScratch[vertID]++;
                }
                else if (boneScratch[vertID] == 1)
                {
                    boneIDs[vertID].y = j;
                    boneWeights[vertID].y = weight;
                    boneScratch[vertID]++;
                }
                else if (boneScratch[vertID] == 2)
                {
                    boneIDs[vertID].z = j;
                    boneWeights[vertID].z = weight;
                    boneScratch[vertID]++;
                }
                else
                {
                    boneIDs[vertID].w = j;
                    boneWeights[vertID].w = weight;
                    boneScratch[vertID]++;
                }
            }
        }

        InitVertexObjects(i, pos, norm, uv, idcs, boneIDs, boneWeights);
    }
}

/**
 * Draw
 */

void SkeletalMesh::Draw()
{
    for (uint32_t i = 0; i < subMeshes.size(); i++)
    {
        glBindVertexArray(subMeshes[i].vaoID);
        glDrawElements(GL_TRIANGLES, subMeshes[i].numIdcs, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}