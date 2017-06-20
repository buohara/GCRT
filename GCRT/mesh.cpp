#include "Mesh.h"

/**
 * [Mesh::InitVertexObjects description]
 * @param subMeshIdx [description]
 * @param pos        [description]
 * @param norms      [description]
 * @param uvs        [description]
 * @param tans       [description]
 */

void Mesh::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos, 
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<vec3> &tans
)
{
    uint32_t numVerts = (uint32_t)pos.size();

    size_t posBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);
    size_t tanBufSize = 3 * numVerts * sizeof(float);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &subMeshes[subMeshIdx].tanVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].tanVboID);
    glBufferData(GL_ARRAY_BUFFER, tanBufSize, &tans[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

/**
 * [Mesh::InitVertexObjects description]
 * @param subMeshIdx [description]
 * @param pos        [description]
 * @param norms      [description]
 * @param uvs        [description]
 */

void Mesh::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs
)
{
    uint32_t numVerts = (uint32_t)pos.size();

    size_t posBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

/**
 * [Mesh::InitVertexObjects description]
 * @param subMeshIdx [description]
 * @param pos        [description]
 * @param norms      [description]
 * @param uvs        [description]
 * @param idcs       [description]
 */

void Mesh::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<uint32_t> &idcs
)
{
    uint32_t numVerts = (uint32_t)pos.size();
    uint32_t numIdcs = (uint32_t)idcs.size();

    size_t posBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);
    size_t idxBufSize = numIdcs * sizeof(uint32_t);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &subMeshes[subMeshIdx].idxVboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[subMeshIdx].idxVboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBufSize, &idcs[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

/**
 * [Mesh::InitVertexObjects description]
 * @param subMeshIdx  [description]
 * @param pos         [description]
 * @param norms       [description]
 * @param uvs         [description]
 * @param idcs        [description]
 * @param boneIDs     [description]
 * @param boneWeights [description]
 */

void Mesh::InitVertexObjects(
    uint32_t subMeshIdx,
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<uint32_t> &idcs,
    vector<ivec4> &boneIDs,
    vector<vec4> &boneWeights
)
{
    uint32_t numVerts = (uint32_t)pos.size();
    uint32_t numIdcs = (uint32_t)idcs.size();

    size_t posBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);
    size_t idxBufSize = numIdcs * sizeof(uint32_t);
    size_t boneIDBufSize = 4 * numVerts * sizeof(uint32_t);
    size_t boneWtBufSize = 4 * numVerts * sizeof(float);

    glGenVertexArrays(1, &subMeshes[subMeshIdx].vaoID);
    glBindVertexArray(subMeshes[subMeshIdx].vaoID);

    glGenBuffers(1, &subMeshes[subMeshIdx].posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &subMeshes[subMeshIdx].normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &subMeshes[subMeshIdx].uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneIDVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneIDVboID);
    glBufferData(GL_ARRAY_BUFFER, boneIDBufSize, &boneIDs[0], GL_STATIC_DRAW);
    glVertexAttribIPointer((GLuint)4, 4, GL_INT, 0, 0);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &subMeshes[subMeshIdx].boneWtVboID);
    glBindBuffer(GL_ARRAY_BUFFER, subMeshes[subMeshIdx].boneWtVboID);
    glBufferData(GL_ARRAY_BUFFER, boneWtBufSize, &boneWeights[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(5);

    glGenBuffers(1, &subMeshes[subMeshIdx].idxVboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[subMeshIdx].idxVboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBufSize, &idcs[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}