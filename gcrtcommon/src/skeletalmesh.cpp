#include "skeletalmesh.h"

/**
 * aiMatrix4x4ToGlm Convert an Assimp matrix to GLM matrix.
 *
 * @param  from Assimp matrix to convert (in).
 * @return      Equivalent GLM matrix.
 */

mat4 aiMatrix4x4ToGlm(aiMatrix4x4& from)
{
    glm::mat4 to;

    to[0][0] = (float)from.a1;
    to[0][1] = (float)from.b1;  
    to[0][2] = (float)from.c1; 
    to[0][3] = (float)from.d1;
    to[1][0] = (float)from.a2; 
    to[1][1] = (float)from.b2;
    to[1][2] = (float)from.c2;
    to[1][3] = (float)from.d2;
    to[2][0] = (float)from.a3; 
    to[2][1] = (float)from.b3;
    to[2][2] = (float)from.c3;
    to[2][3] = (float)from.d3;
    to[3][0] = (float)from.a4;
    to[3][1] = (float)from.b4;
    to[3][2] = (float)from.c4;
    to[3][3] = (float)from.d4;

    return to;
}

/**
 * LoadAnimations Top-level call into skeleton tree to load a bone's
 * keyframes. Load first animation in file only.
 
 * Each Assimp animation has a list of "channels", each of which is a sequence of keyframes 
 * assigned to a bone in the mesh skelton. Loop through each bone channel and pass it to the 
 * skeleton root, which will recursively find the appropriate bone and load its keyframes.
 *
 * @param scene Assimp scene.
 * @param root Root of the skeleton.
 */

void LoadAnimations(
    const aiScene &scene,
    BoneTreeNode &root
)
{
    for (uint32_t i = 0; i < scene.mAnimations[0]->mNumChannels; i++)
    {
        root.LoadAnimation(*scene.mAnimations[0]->mChannels[i]);
    }
}

/**
 * BoneTreeNode::LoadAnimation Recurse through mesh skeleton and find bone associated with
 * given set of keyframes. When match is found, load up the keyframes.
 *
 * @param  anim Set of keyframes for a given animation for a particular bone (in).
 * @return      True if matching bone was found and keyframes loaded.
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
            if (children[i].LoadAnimation(anim))
            {
                return true;
            }
        }
    }

    return false;
}

/**
 * CreateBoneHierarchy Recursively generate the bone tree structure for
 * given input mesh. For each input node, add all its child nodes and offset from parent
 * node. Then recurse into each child node and pass its child information.
 *
 * @param aiNode      Current Assimp node in skeleton hierarchy (in).
 * @param btNode      Current node in GCRT skeleton (in).
 * @param boneOffsets List of each bone's offset from mesh space to its local one space (in).
 */

void CreateBoneHierarchy(
    aiNode &aiNode, 
    BoneTreeNode &btNode,
    map<string, mat4> &boneOffsets
)
{
    for (uint32_t i = 0; i < aiNode.mNumChildren; i++)
    {
        BoneTreeNode child;
        child.name = aiNode.mChildren[i]->mName.C_Str();

        if (boneOffsets.find(child.name) != boneOffsets.end()) child.boneOffset = boneOffsets[child.name];
        else child.boneOffset = mat4(1.0);

        child.parentOffset = aiMatrix4x4ToGlm(aiNode.mChildren[i]->mTransformation);

        btNode.children.push_back(child);
        
        CreateBoneHierarchy(
            *aiNode.mChildren[i],
            btNode.children[i],
            boneOffsets
        );
    }
}

/**
 * LoadVertexData Load an Assimp mesh's vertex data (positions,
 * normals, UVs, and triangle indices).
 *
 * @param mesh Input assimp mesh (in)
 * @param pos  Vertex positions (out).
 * @param norm Vertex normals (out).
 * @param UV   Vertex UVs (out).
 * @param idcs Triangle vertex indices (out).
 */

void LoadVertexData(
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
    idcs.resize(3 * mesh.mNumFaces);

    // Vertex attributes.

    for (uint32_t j = 0; j < mesh.mNumVertices; j++)
    {
        pos[j].x = mesh.mVertices[j].x;
        pos[j].y = mesh.mVertices[j].y;
        pos[j].z = mesh.mVertices[j].z;

        norm[j].x = mesh.mNormals[j].x;
        norm[j].y = mesh.mNormals[j].y;
        norm[j].z = mesh.mNormals[j].z;

        if (mesh.mNumUVComponents[0] > 0)
        {
            UV[j].x = mesh.mTextureCoords[0][j].x;
            UV[j].y = 1.0f - mesh.mTextureCoords[0][j].y;
        }
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
 * LoadMaterials
 *
 * @param mesh        Assimp mesh (in).
 */

void LoadMaterials(const aiScene &scene)
{
    for (uint32_t i = 0; i < scene.mNumMaterials; i++)
    {
        aiMaterial &mat = *scene.mMaterials[i];
        aiString name;
        aiColor4D kd;
        aiColor4D ka;
        aiColor4D ks;
        uint32_t numTextures;
        float shininess;

        if (mat.Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS)
        {
        }

        if (mat.Get(AI_MATKEY_COLOR_DIFFUSE, kd) == aiReturn_SUCCESS)
        {
        }

        if (mat.Get(AI_MATKEY_COLOR_AMBIENT, ka) == aiReturn_SUCCESS)
        {
        }

        if (mat.Get(AI_MATKEY_COLOR_SPECULAR, ks) == aiReturn_SUCCESS)
        {
        }
        
        if (mat.Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS)
        {
        }

        if ((numTextures = mat.GetTextureCount(aiTextureType_DIFFUSE)) > 0)
        {

        }

        __debugbreak();
    }

    return;
}

/**
 * LoadBoneData Every vertex in the mesh is influenced by a
 * collection of bones. Loop through each bone in the matrix and load data about
 * which vertices it influences.
 *
 * @param mesh        Assimp mesh (in).
 * @param boneIDs     For each vertex, a list of bone IDs that influence it (out).
 * @param boneWts     For each vertex, the strength of each influencing bone (out).
 * @param boneOffsets Offset of bone from mesh space to local bone space (out).
 */

void LoadBoneData(
    aiMesh &mesh,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWts,
    map<string, mat4> &boneOffsets,
    map<string, uint32_t> &boneMap
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
 * UpdateAnimation Get a list of bone animation matrices interpolated
 * at input time t.
 *
 * @param root          Root of a skeletal mesh.
 * @param t             Requested animation time (in).
 * @param rootTrans     Global offset in world space of whole mesh (in).
 * @param bones         List of bone animation matrices at time t (out).
 * @param boneMap       Bone name to bone index map.
 */

void GetAnimation(
    BoneTreeNode &root,
    float t,
    mat4 rootTrans,
    vector<mat4> &bones,
    map<string, uint32_t> &boneMap
)
{
    if (bones.size() != boneMap.size()) bones.resize(boneMap.size());
    root.GetBoneMatrices(t, bones, rootTrans, boneMap);
}

/**
 * BoneTreeNode::GetBoneMatrices Recursively build bone animation matrices at input
 * time t using overall bone mesh offset and parent offset.
 *
 * @param t        Animation time to interpolate (in).
 * @param matrices List of bone matrices at time t (out).
 * @param parent   Current bone's parent bone matrix at time t (in).
 * @param boneMap  Map of bone names/indices into output list of bone matrices (in).
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

        for (uint32_t i = 0; i < children.size(); i++) children[i].GetBoneMatrices(t, matrices, parent * boneMat, boneMap);
    }
    else
    {
        for (uint32_t i = 0; i < children.size(); i++) children[i].GetBoneMatrices(t, matrices, parent* boneMat, boneMap);
    }
}