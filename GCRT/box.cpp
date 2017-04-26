#include "box.h"

/**
 * Create - Create a box.
 *
 * @param dims Box x, y, and z dimensions.
 * @param pos Initial box position.
 * @param color Box color when you shine white light on it.
 * @param progID Shader program handle for setting shader parameters
 * on on draw.
 */

void Box::Create(GLuint progID)
{
    shaderProgID = progID;
    kd = vec3(0.5, 0.5, 0.5);

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

    numVerts = verts.size();

    InitVertexObjects(verts, norms);
    InitModelMatrices();
}

/**
 * Draw - Draw the box. Set it's model matrices and color in the shader.
 */

void Box::Draw()
{
    GLuint modelID = glGetUniformLocation(shaderProgID, "model");
    glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

    GLuint modelInvID = glGetUniformLocation(shaderProgID, "modelInv");
    glUniformMatrix4fv(modelInvID, 1, false, &modelInv[0][0]);

    GLuint kdID = glGetUniformLocation(shaderProgID, "kd");
    glUniform3fv(kdID, 1, &kd[0]);

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);
}