#include "geometry.h"

/**
 * InitVertexObjects -
 *
 * @param verts
 * @param norms
 */

void Geometry::InitVertexObjects(
    vector<vec3> &verts, 
    vector<vec3> &norms,
    vector<vec2> &uvs)
{
    uint32_t numVerts = verts.size();

    size_t vertBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);
    size_t uvBufSize = 2 * numVerts * sizeof(float);

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &vertVboID);
    glBindBuffer(GL_ARRAY_BUFFER, vertVboID);
    glBufferData(GL_ARRAY_BUFFER, vertBufSize, &verts[0], GL_STATIC_DRAW);
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

void Geometry::SetUniforms()
{
    GLuint modelID = glGetUniformLocation(shaderProgID, "model");
    glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

    GLuint modelInvID = glGetUniformLocation(shaderProgID, "modelInv");
    glUniformMatrix4fv(modelInvID, 1, false, &modelInv[0][0]);

    GLuint kdID = glGetUniformLocation(shaderProgID, "kd");
    glUniform3fv(kdID, 1, &kd[0]);

    GLuint kaID = glGetUniformLocation(shaderProgID, "ka");
    glUniform3fv(kaID, 1, &ka[0]);
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

void Geometry::Draw()
{
    return;
}