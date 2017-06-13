#include "Mesh.h"

/**
 * InitVertexObjects -
 *
 * @param verts
 * @param norms
 * @param norms
 * @param tans
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
* InitVertexObjects -
*
* @param verts
* @param norms
* @param norms
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
 * InitVertexObjects -
 *
 * @param verts
 * @param norms
 * @param norms
 * @param idcs
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