#include "geometry.h"

/**
 * InitVertexObjects -
 *
 * @param verts
 * @param norms
 * @param norms
 * @param tans
 */

void Geometry::InitVertexObjects(
    vector<vec3> &pos, 
    vector<vec3> &norms,
    vector<vec2> &uvs,
    vector<vec3> &tans
)
{
    uint32_t numVerts = pos.size();

    size_t posBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);
    size_t tanBufSize = 3 * numVerts * sizeof(float);

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &tanVboID);
    glBindBuffer(GL_ARRAY_BUFFER, tanVboID);
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

void Geometry::InitVertexObjects(
    vector<vec3> &pos,
    vector<vec3> &norms,
    vector<vec2> &uvs
)
{
    uint32_t numVerts = pos.size();

    size_t posBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &posVboID);
    glBindBuffer(GL_ARRAY_BUFFER, posVboID);
    glBufferData(GL_ARRAY_BUFFER, posBufSize, &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &uvVboID);
    glBindBuffer(GL_ARRAY_BUFFER, uvVboID);
    glBufferData(GL_ARRAY_BUFFER, uvBufSize, &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

/**
 * SetUniforms -
 */

void Geometry::SetShaderParams(GLuint progID)
{
    GLuint modelID = glGetUniformLocation(progID, "model");
    glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

    GLuint modelInvID = glGetUniformLocation(progID, "modelInv");
    glUniformMatrix4fv(modelInvID, 1, false, &modelInv[0][0]);
}

/**
 * InitVertexObjects -
 */

void Geometry::InitModelMatrices()
{
    scl = mat4(1.0f);
    rot = mat4(1.0f);
    trans = mat4(1.0f);

    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * Translate -
 */

void Geometry::Translate(vec3 tx)
{
    trans = translate(tx);
    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * Scale -
 */

void Geometry::Scale(vec3 dims)
{
    scl = scale(mat4(1.0f), dims);
    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * Draw -
 */

//void Geometry::Draw()
//{
//    return;
//}