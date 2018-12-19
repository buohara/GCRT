#include "skeletalmesh.h"

mat4 globalInverse;

/**
 * aiMatrix4x4ToGlm Convert an Assimp matrix to GLM matrix.
 *
 * @param  from Assimp matrix to convert.
 * @return      Equivalent GLM matrix.
 */

inline mat4 aiMatrix4x4ToGlm(aiMatrix4x4& from)
{
    glm::mat4 to;

    to[0][0] = (GLfloat)from.a1;
    to[0][1] = (GLfloat)from.b1;  
    to[0][2] = (GLfloat)from.c1; 
    to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; 
    to[1][1] = (GLfloat)from.b2;
    to[1][2] = (GLfloat)from.c2;
    to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; 
    to[2][1] = (GLfloat)from.b3;
    to[2][2] = (GLfloat)from.c3;
    to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4;
    to[3][1] = (GLfloat)from.b4;
    to[3][2] = (GLfloat)from.c4;
    to[3][3] = (GLfloat)from.d4;

    return to;
}

/**
 * SkeletalMesh::Create Load an Assimp mesh, including animations, from file.
 *
 * @param file Assimp mesh file.
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
 * SkeletalMesh::LoadAnimations Load animations from an Assimp scene.
 *
 * @param scene Assimp scene.
 */

void SkeletalMesh::LoadAnimations(const aiScene &scene)
{
    for (uint32_t i = 0; i < scene.mAnimations[0]->mNumChannels; i++)
    {
        root.LoadAnimation(*scene.mAnimations[0]->mChannels[i]);
    }
}

/**
 * [BoneTreeNode::LoadAnimation description]
 * @param  anim [description]
 * @return      [description]
 */

bool BoneTreeNode::LoadAnimation(aiNodeAnim &anim)
{
    if (name == anim.mNodeName.C_Str())
    {
        for (uint32_t i = 0; i < anim.mNumPositionKeys; i++)
        {
            aiVectorKey posKey = anim.mPositionKeys[i];
            vec3 pos(posKey.mValue.x, posKey.mValue.y, posKey.mValue.z);

            aiQuatKey rotKey = anim.mRotationKeys[i];
            quat rot(rotKey.mValue.w, rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z);

            aiVectorKey scaleKey = anim.mScalingKeys[0];
            vec3 scale(scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z);

            float t = (float)posKey.mTime;

            KeyFrame kf(t, rot, pos, scale);
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
 * [SkeletalMesh::CreateBoneHierarchy description]
 * @param aiNode      [description]
 * @param btNode      [description]
 * @param boneOffsets [description]
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
 * [SkeletalMesh::LoadVertexAndBoneData description]
 * @param scene       [description]
 * @param boneOffsets [description]
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
        vector<ivec4> boneIDs;
        vector<vec4> boneWeights;

        subMeshes[i].numVerts = mesh.mNumVertices;
        subMeshes[i].numIdcs = 3 * mesh.mNumFaces;

        LoadVertexData(mesh, pos, norm, uv, idcs);

        if (animated == false)
        {
            boneIDs.push_back(ivec4(0));
            boneIDs.push_back(vec4(1.0f, 0.0f, 0.0f, 0.0f));

            InitVertexObjects(i, pos, norm, uv, idcs, boneIDs, boneWeights);
            continue;
        }

        // Bone IDs and weights.

        LoadBoneData(mesh, boneIDs, boneWeights, boneOffsets);
        InitVertexObjects(i, pos, norm, uv, idcs, boneIDs, boneWeights);
    }
}

/**
 * [SkeletalMesh::LoadVertexData description]
 * @param mesh [description]
 * @param pos  [description]
 * @param norm [description]
 * @param UV   [description]
 * @param idcs [description]
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
 * [SkeletalMesh::LoadBoneData description]
 * @param mesh        [description]
 * @param boneIDs     [description]
 * @param boneWts     [description]
 * @param boneOffsets [description]
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
            boneID = (uint32_t)boneMap.size();
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
 * [SkeletalMesh::UpdateAnimation description]
 * @param t             [description]
 */

void SkeletalMesh::GetAnimation(float t, mat4 rootTrans, vector<mat4> &bones)
{
    if (animated == true)
    {
        if (bones.size() != boneMap.size())
        {
            bones.resize(boneMap.size());
        }

        root.GetBoneMatrices(t, bones, rootTrans, boneMap);
    }
    else
    {
        if (bones.size() != 1)
        {
            bones.resize(1);
        }

        bones[0] = rootTrans;
    }
}

/**
 * [BoneTreeNode::GetBoneMatrices description]
 * @param t        [description]
 * @param matrices [description]
 * @param parent   [description]
 * @param boneMap  [description]
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
 * [SkeletalMesh::Draw description]
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