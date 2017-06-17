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
    map<string, mat4> boneOffsets;

    LoadVertexAndBoneData(scene, boneOffsets);
    
    aiNode &scnRoot = *(scene.mRootNode);
    root.name = scnRoot.mName.C_Str();
    CreateBoneHierarchy(scnRoot, root, boneOffsets);

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
            quat q;
            q.x = rotKey.mValue.x;
            q.y = rotKey.mValue.y;
            q.z = rotKey.mValue.z;
            q.w = -rotKey.mValue.w;
            mat4 rot = mat4_cast(q);

            aiVectorKey scaleKey = anim.mScalingKeys[0];
            vec3 scalev;
            scalev.x = scaleKey.mValue.x;
            scalev.y = scaleKey.mValue.y;
            scalev.z = scaleKey.mValue.z;
            mat4 scal = scale(scalev);

            KeyFrame kf(t, trans * rot * scal);
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

void SkeletalMesh::CreateBoneHierarchy(
    aiNode &aiNode, 
    BoneTreeNode &btNode,
    map<string, mat4> boneOffsets
)
{
    for (uint32_t i = 0; i < aiNode.mNumChildren; i++)
    {
        BoneTreeNode child;
        child.name = aiNode.mChildren[i]->mName.C_Str();

        if (boneOffsets.find(child.name) != boneOffsets.end())
        {
            child.boneOffset = boneOffsets[child.name];
        }
        else
        {
            child.boneOffset = mat4(1.0);
        }

        btNode.children.push_back(make_shared<BoneTreeNode>(child));
        
        CreateBoneHierarchy(
            *aiNode.mChildren[i],
            *btNode.children[i],
            boneOffsets
        );
    }
}

/**
 * LoadVertexAndBoneData - 
 */

void SkeletalMesh::LoadVertexAndBoneData(
    const aiScene &scene,
    map<string, mat4> boneOffsets
)
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

        LoadBoneData(mesh, boneIDs, boneWeights, boneOffsets);
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
    vector<vec4> &boneWts,
    map<string, mat4> boneOffsets
)
{
    vector<uint32_t> boneScratch(mesh.mNumVertices, 0);
    boneIDs.resize(mesh.mNumVertices);
    boneWts.resize(mesh.mNumVertices);

    for (uint32_t j = 0; j < mesh.mNumBones; j++)
    {
        aiBone &bone = *(mesh.mBones[j]);
        uint32_t boneID;

        if (boneMap.find(bone.mName.C_Str()) == boneMap.end())
        {
            boneID = boneMap.size();
            boneMap[bone.mName.C_Str()] = boneID;
            
            mat4 boneOffset;
            memcpy(&boneOffset[0], &bone.mOffsetMatrix.Transpose(), 16 * sizeof(float));
            boneOffsets[bone.mName.C_Str()] = boneOffset;
        }
        else
        {
            boneID = boneMap[bone.mName.C_Str()];
        }

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
    root.GetBoneMatrices(t, boneMats, scale(vec3(0.1f, 0.1f, 0.1f)), boneMap);

    GLuint bonesID = glGetUniformLocation(renderProgram, "bones");
    glUniformMatrix4fv(bonesID, numBones, false, value_ptr(boneMats[0]));

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
    if (boneMap.find(name) != boneMap.end())
    {
        uint32_t boneIdx = boneMap[name];
        mat4 boneMat = animation.GetAnimationMatrix(t);
        boneMat = parent * boneMat;

        matrices[boneIdx] = boneMat * boneOffset;

        for (uint32_t i = 0; i < children.size(); i++)
        {
            children[i]->GetBoneMatrices(t, matrices, boneMat, boneMap);
        }
    }
    else
    {
        for (uint32_t i = 0; i < children.size(); i++)
        {
            children[i]->GetBoneMatrices(t, matrices, parent, boneMap);
        }
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