#include "blendermesh.h"

/**
 * Create
 */

void BlenderMesh::Create(string file)
{
    Assimp::Importer importer;
    const aiScene *pScene = importer.ReadFile(file, aiProcessPreset_TargetRealtime_Fast);
    aiMesh *pMesh = pScene->mMeshes[0];

    vector<vec3> pos;
    vector<vec3> norm;
    vector<vec2> uv;

    numVerts = 3 * pMesh->mNumFaces;

    pos.resize(numVerts);
    norm.resize(numVerts);
    uv.resize(numVerts);

    for (uint32_t i = 0; i < numVerts; i++)
    {
        pos[i].x = pMesh->mVertices[i].x;
        pos[i].y = pMesh->mVertices[i].y;
        pos[i].z = pMesh->mVertices[i].z;

        norm[i].x = pMesh->mNormals[i].x;
        norm[i].y = pMesh->mNormals[i].y;
        norm[i].z = pMesh->mNormals[i].z;

        uv[i].x = pMesh->mTextureCoords[0][i].x;
        uv[i].y = pMesh->mTextureCoords[0][i].y;
    }

    InitVertexObjects(pos, norm, uv);
}

/**
 * Draw
 */

void BlenderMesh::Draw()
{
    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);
}