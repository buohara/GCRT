#include "skeletalmesh.h"

/**
 * Create
 */

mat4 globalInverse;

inline mat4 aiMatrix4x4ToGlm(aiMatrix4x4& from)
{
    glm::mat4 to;

    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

    return to;
}

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

    globalInverse = aiMatrix4x4ToGlm(scnRoot.mTransformation.Inverse());

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
            aiMatrix3x3t<float> aiMat = rotKey.mValue.GetMatrix();
            mat4 rot = mat4(1.0);

            rot[0][0] = aiMat.a1;
            rot[0][1] = aiMat.b1;
            rot[0][2] = aiMat.c1;
            rot[1][0] = aiMat.a2;
            rot[1][1] = aiMat.b2;
            rot[1][2] = aiMat.c2;
            rot[2][0] = aiMat.a3;
            rot[2][1] = aiMat.b3;
            rot[2][2] = aiMat.c3;

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
    map<string, mat4> &boneOffsets
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

        child.parentOffset = aiMatrix4x4ToGlm(aiNode.mChildren[i]->mTransformation);

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
    map<string, mat4> &boneOffsets
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

        vector<ivec4> boneIDs;
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
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWts,
    map<string, mat4> &boneOffsets
)
{
    vector<uint32_t> boneScratch(mesh.mNumVertices, 0);
    boneIDs.resize(mesh.mNumVertices, ivec4(0));
    boneWts.resize(mesh.mNumVertices, vec4(0.0));

    for (uint32_t j = 0; j < mesh.mNumBones; j++)
    {
        aiBone &bone = *(mesh.mBones[j]);
        uint32_t boneID;

        if (boneMap.find(bone.mName.C_Str()) == boneMap.end())
        {
            boneID = boneMap.size();
            boneMap[bone.mName.C_Str()] = boneID;
            
            mat4 boneOffset;
            boneOffset = aiMatrix4x4ToGlm(bone.mOffsetMatrix);          
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
    root.GetBoneMatrices(t, boneMats, mat4(1.0f), boneMap);

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
    mat4 boneMat = parentOffset;

    if (boneMap.find(name) != boneMap.end())
    {
        uint32_t boneIdx = boneMap[name];
        mat4 boneMat = animation.GetAnimationMatrix(t);
        
        matrices[boneIdx] = parent * boneMat * boneOffset;

        for (uint32_t i = 0; i < children.size(); i++)
        {
            children[i]->GetBoneMatrices(t, matrices, parent * boneMat, boneMap);
        }
    }
    else
    {
        for (uint32_t i = 0; i < children.size(); i++)
        {
            children[i]->GetBoneMatrices(t, matrices, parent * boneMat, boneMap);
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