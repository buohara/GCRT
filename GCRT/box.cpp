#include "box.h"

void Box::Create(
    vec3 dims, 
    vec3 pos,
    vec3 color,
    GLuint progID)
{
    shaderProgID = progID;
    clr = color;

    vector<vec3> verts;
    vector<vec3> norms;

    float hx = 0.5;
    float hy = 0.5;
    float hz = 0.5;

    // Front

    verts.push_back(vec3(hx, hy, hz));
    verts.push_back(vec3(hx, -hy, hz));
    verts.push_back(vec3(hx, -hy, -hz));

    verts.push_back(vec3(hx, hy, hz));
    verts.push_back(vec3(hx, -hy, -hz));
    verts.push_back(vec3(hx, hy, -hz));

    norms.push_back(vec3(1.0, 0.0, 0.0));
    norms.push_back(vec3(1.0, 0.0, 0.0));
    norms.push_back(vec3(1.0, 0.0, 0.0));

    norms.push_back(vec3(1.0, 0.0, 0.0));
    norms.push_back(vec3(1.0, 0.0, 0.0));
    norms.push_back(vec3(1.0, 0.0, 0.0));

    // Back

    verts.push_back(vec3(-hx, -hy, hz));
    verts.push_back(vec3(-hx, hy, hz));
    verts.push_back(vec3(-hx, hy, -hz));

    verts.push_back(vec3(-hx, -hy, hz));
    verts.push_back(vec3(-hx, hy, -hz));
    verts.push_back(vec3(-hx, -hy, -hz));

    norms.push_back(vec3(-1.0, 0.0, 0.0));
    norms.push_back(vec3(-1.0, 0.0, 0.0));
    norms.push_back(vec3(-1.0, 0.0, 0.0));

    norms.push_back(vec3(-1.0, 0.0, 0.0));
    norms.push_back(vec3(-1.0, 0.0, 0.0));
    norms.push_back(vec3(-1.0, 0.0, 0.0));

    // Right

    verts.push_back(vec3(-hx, hy, hz));
    verts.push_back(vec3(hx, hy, hz));
    verts.push_back(vec3(hx, hy, -hz));

    verts.push_back(vec3(-hx, hy, hz));
    verts.push_back(vec3(hx, hy, -hz));
    verts.push_back(vec3(-hx, hy, -hz));

    norms.push_back(vec3(0.0, 1.0, 0.0));
    norms.push_back(vec3(0.0, 1.0, 0.0));
    norms.push_back(vec3(0.0, 1.0, 0.0));

    norms.push_back(vec3(0.0, 1.0, 0.0));
    norms.push_back(vec3(0.0, 1.0, 0.0));
    norms.push_back(vec3(0.0, 1.0, 0.0));

    // Left

    verts.push_back(vec3(hx, -hy, hz));
    verts.push_back(vec3(-hx, -hy, hz));
    verts.push_back(vec3(-hx, -hy, -hz));

    verts.push_back(vec3(hx, -hy, hz));
    verts.push_back(vec3(-hx, -hy, -hz));
    verts.push_back(vec3(hx, -hy, -hz));

    norms.push_back(vec3(0.0, -1.0, 0.0));
    norms.push_back(vec3(0.0, -1.0, 0.0));
    norms.push_back(vec3(0.0, -1.0, 0.0));

    norms.push_back(vec3(0.0, -1.0, 0.0));
    norms.push_back(vec3(0.0, -1.0, 0.0));
    norms.push_back(vec3(0.0, -1.0, 0.0));

    // Top

    verts.push_back(vec3(hx, hy, hz));
    verts.push_back(vec3(-hx, hy, hz));
    verts.push_back(vec3(-hx, -hy, hz));

    verts.push_back(vec3(hx, hy, hz));
    verts.push_back(vec3(-hx, -hy, hz));
    verts.push_back(vec3(hx, -hy, hz));

    norms.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0.0, 0.0, 1.0));

    norms.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0.0, 0.0, 1.0));

    // Bottom

    verts.push_back(vec3(-hx, hy, -hz));
    verts.push_back(vec3(hx, hy, -hz));
    verts.push_back(vec3(hx, -hy, -hz));

    verts.push_back(vec3(-hx, hy, -hz));
    verts.push_back(vec3(hx, -hy, -hz));
    verts.push_back(vec3(-hx, -hy, -hz));

    norms.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0.0, 0.0, -1.0));

    norms.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0.0, 0.0, -1.0));

    numVerts = 36;

    // Create GPU vertex buffers and upload data.

    size_t vertBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);

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

    glBindVertexArray(0);

    // Initialize this box's model matrix.

    mat4 sc = scale(mat4(1.0), vec3(dims.x, dims.y, dims.z));
    mat4 rot = mat4(1.0);
    mat4 trans = translate(vec3(pos.x, pos.y, pos.z));

    model = trans * rot * sc;
    modelInv = inverseTranspose(model);
}

void Box::Draw()
{
    GLuint modelID = glGetUniformLocation(shaderProgID, "model");
    glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

    GLuint modelInvID = glGetUniformLocation(shaderProgID, "modelInv");
    glUniformMatrix4fv(modelInvID, 1, false, &modelInv[0][0]);

    GLuint kdID = glGetUniformLocation(shaderProgID, "kd");
    glUniform3fv(kdID, 1, &clr[0]);

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);
}