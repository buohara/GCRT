#include "skeletalmesh.h"

/**
 * Create
 */

void SkeletalMesh::Create(string file)
{
    Assimp::Importer importer;
    
    const aiScene &scene = *(importer.ReadFile(
        file, 
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
    ));
 
    subMeshes.resize(scene.mNumMeshes);
    animated = (scene.mNumAnimations > 0);

    LoadVertexAndBoneData(scene);
    
    aiNode &scnRoot = *(scene.mRootNode);
    root.name = scnRoot.mName.C_Str();
    CreateBoneHierarchy(scnRoot, root);

    LoadAnimations(scene);
}

/**
 * LoadAnimations -
 */

void SkeletalMesh::LoadAnimations(const aiScene &scene)
{
    for (uint32_t i = 0; i < scene.mAnimations[0]->mNumChannels; i++)
    {
        root.LoadAnimation(*scene.mAnimations[0]->mChannels[i]);
    }
}

/**
 * LoadAnimation -
 */

bool BoneTreeNode::LoadAnimation(aiNodeAnim &anim)
{
    if (name == anim.mNodeName.C_Str())
    {
        for (uint32_t i = 0; i < anim.mNumPositionKeys; i++)
        {
            aiVectorKey posKey = anim.mPositionKeys[i];
            float t = posKey.mTime;

            vec3 pos;
            pos.x = posKey.mValue.x;
            pos.y = posKey.mValue.y;
            pos.z = posKey.mValue.z;
            mat4 trans = translate(pos);

            aiQuatKey rotKey = anim.mRotationKeys[i];
            tquat<float> quat;
            quat.x = rotKey.mValue.x;
            quat.y = rotKey.mValue.y;
            quat.z = rotKey.mValue.z;
            quat.y = rotKey.mValue.w;

            mat4 rot = toMat4(quat);
            KeyFrame kf(t, trans * rot);
            animation.AddKF(kf);
        }

        return true;
    }
    else
    {
        for (uint32_t i = 0; i < children.size(); i++)
        {
            if (children[i]->LoadAnimation(anim))
            {
                return true;
            }
        }
    }

    return false;
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
        child.boneOffset = inverse(child.boneOffset);

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

        LoadVertexData(mesh, pos, norm, uv, idcs);

        if (animated == false)
        {
            InitVertexObjects(i, pos, norm, uv, idcs);
            continue;
        }

        // Bone IDs and weights.

        vector<uvec4> boneIDs;
        vector<vec4> boneWeights;
        LoadBoneData(mesh, boneIDs, boneWeights);

        InitVertexObjects(i, pos, norm, uv, idcs, boneIDs, boneWeights);
    }
}

/**
 * LoadVertexData - 
 */

void SkeletalMesh::LoadVertexData(
    aiMesh &mesh,
    vector<vec3> &pos,
    vector<vec3> &norm,
    vector<vec2> &UV,
    vector<uint32_t> &idcs
)
{
    pos.resize(mesh.mNumVertices);
    norm.resize(mesh.mNumVertices);
    UV.resize(mesh.mNumVertices);
    idcs.resize(mesh.mNumVertices);

    // Vertex attributes.

    for (uint32_t j = 0; j < mesh.mNumVertices; j++)
    {
        pos[j].x = mesh.mVertices[j].x;
        pos[j].y = mesh.mVertices[j].y;
        pos[j].z = mesh.mVertices[j].z;

        norm[j].x = mesh.mNormals[j].x;
        norm[j].y = mesh.mNormals[j].y;
        norm[j].z = mesh.mNormals[j].z;

        UV[j].x = mesh.mTextureCoords[0][j].x;
        UV[j].y = 1.0f - mesh.mTextureCoords[0][j].y;
    }

    // Triangle indices.

    for (uint32_t j = 0; j < mesh.mNumFaces; j++)
    {
        idcs[3 * j] = mesh.mFaces[j].mIndices[0];
        idcs[3 * j + 1] = mesh.mFaces[j].mIndices[1];
        idcs[3 * j + 2] = mesh.mFaces[j].mIndices[2];
    }
}

/**
 * LoadBoneData -
 */

void SkeletalMesh::LoadBoneData(
    aiMesh &mesh,
    vector<uvec4> &boneIDs,
    vector<vec4> &boneWts
)
{
    vector<uint32_t> boneScratch(mesh.mNumVertices, 0);
    boneIDs.resize(mesh.mNumVertices);
    boneWts.resize(mesh.mNumVertices);

    for (uint32_t j = 0; j < mesh.mNumBones; j++)
    {
        aiBone &bone = *(mesh.mBones[j]);
        uint32_t boneID = boneMap.size();
        boneMap[bone.mName.C_Str()] = boneID;

        for (uint32_t k = 0; k < mesh.mBones[j]->mNumWeights; k++)
        {
            uint32_t vertID = bone.mWeights[k].mVertexId;
            float weight = bone.mWeights[k].mWeight;

            if (boneScratch[vertID] == 0)
            {
                boneIDs[vertID].x = boneID;
                boneWts[vertID].x = weight;
                boneScratch[vertID]++;
            }
            else if (boneScratch[vertID] == 1)
            {
                boneIDs[vertID].y = boneID;
                boneWts[vertID].y = weight;
                boneScratch[vertID]++;
            }
            else if (boneScratch[vertID] == 2)
            {
                boneIDs[vertID].z = boneID;
                boneWts[vertID].z = weight;
                boneScratch[vertID]++;
            }
            else
            {
                boneIDs[vertID].w = boneID;
                boneWts[vertID].w = weight;
                boneScratch[vertID]++;
            }
        }
    }
}

/**
 * SetAnimationMatrices
 */

void SkeletalMesh::SetBoneMatrices(float t, GLuint renderProgram)
{
    uint32_t numBones = boneMap.size();
    vector<mat4> boneMats(numBones);
    root.GetBoneMatrices(t, boneMats, mat4(0.1), boneMap);

    GLuint bonesID = glGetUniformLocation(renderProgram, "bones");
    glUniformMatrix4fv(bonesID, numBones, false, &boneMats[0][0][0]);
}

/**
 * SetAnimationMatrices
 */

void BoneTreeNode::GetBoneMatrices(
    float t,
    vector<mat4> &matrices,
    mat4 parent,
    map<string, uint32_t> &boneMap
)
{
    uint32_t boneIdx = boneMap[name];
    mat4 boneMat = animation.GetAnimationMatrix(t);
    boneMat = parent * boneMat;

    matrices[boneIdx] = mat4(0.1);//boneOffset * boneMat;

    for (uint32_t i = 0; i < children.size(); i++)
    {
        children[i]->GetBoneMatrices(t, matrices, mat4(0.1), boneMap);
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