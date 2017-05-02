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

void Box::Create()
{
    vector<vec3> verts;
    vector<vec3> norms;
    vector<vec2> uvs;

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

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

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

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

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

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

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

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

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

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

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

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

    numVerts = verts.size();

    InitVertexObjects(verts, norms, uvs);
    InitModelMatrices();
}

/**
 * Draw - Draw the box. Set it's model matrices and color in the shader.
 */

void Box::Draw()
{
    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);
}