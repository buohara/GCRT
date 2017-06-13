#include "blendermesh.h"

/**
 * Create
 */

void BlenderMesh::Create(string file)
{
    Assimp::Importer importer;
    const aiScene *pScene = importer.ReadFile(file, aiProcessPreset_TargetRealtime_Fast);
    subMeshes.resize(pScene->mNumMeshes);

    for (uint32 i = 0; i < pScene->mNumMeshes; i++)
    {
        aiMesh *pMesh = pScene->mMeshes[i];

        vector<vec3> pos;
        vector<vec3> norm;
        vector<vec2> uv;
        vector<uint32_t> idcs;

        subMeshes[i].numVerts = pMesh->mNumVertices;
        subMeshes[i].numIdcs = 3 * pMesh->mNumFaces;

        pos.resize(subMeshes[i].numVerts);
        norm.resize(subMeshes[i].numVerts);
        uv.resize(subMeshes[i].numVerts);
        idcs.resize(subMeshes[i].numIdcs);

        for (uint32_t j = 0; j < subMeshes[i].numVerts; j++)
        {
            pos[j].x = pMesh->mVertices[j].x;
            pos[j].y = pMesh->mVertices[j].y;
            pos[j].z = pMesh->mVertices[j].z;

            norm[j].x = pMesh->mNormals[j].x;
            norm[j].y = pMesh->mNormals[j].y;
            norm[j].z = pMesh->mNormals[j].z;

            uv[j].x = pMesh->mTextureCoords[0][j].x;
            uv[j].y = 1.0 - pMesh->mTextureCoords[0][j].y;
        }

        for (uint32_t j = 0; j < pMesh->mNumFaces; j++)
        {
            idcs[3 * j] = pMesh->mFaces[j].mIndices[0];
            idcs[3 * j + 1] = pMesh->mFaces[j].mIndices[1];
            idcs[3 * j + 2] = pMesh->mFaces[j].mIndices[2];
        }

        InitVertexObjects(i, pos, norm, uv, idcs);
    }
}

/**
 * Draw
 */

void BlenderMesh::Draw()
{
    for (uint32_t i = 0; i < subMeshes.size(); i++)
    {
        glBindVertexArray(subMeshes[i].vaoID);
        glDrawElements(GL_TRIANGLES, subMeshes[i].numIdcs, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}