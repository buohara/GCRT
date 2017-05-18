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
    vector<vec3> pos;
    vector<vec3> norms;
    vector<vec2> uvs;
    vector<vec3> tans;

    GenPositions(pos);
    GenNormals(norms);
    GenUVs(uvs);
    GenTans(tans);

    numVerts = pos.size();

    InitVertexObjects(pos, norms, uvs, tans);
    InitModelMatrices();
}

/**
 * GenPositions
 */

void Box::GenPositions(vector<vec3> &pos)
{
    float hx = 1.0;
    float hy = 1.0;
    float hz = 1.0;

    // Front

    pos.push_back(vec3(hx, hy, hz));
    pos.push_back(vec3(hx, -hy, hz));
    pos.push_back(vec3(hx, -hy, -hz));

    pos.push_back(vec3(hx, hy, hz));
    pos.push_back(vec3(hx, -hy, -hz));
    pos.push_back(vec3(hx, hy, -hz));

    // Back

    pos.push_back(vec3(-hx, -hy, hz));
    pos.push_back(vec3(-hx, hy, hz));
    pos.push_back(vec3(-hx, hy, -hz));

    pos.push_back(vec3(-hx, -hy, hz));
    pos.push_back(vec3(-hx, hy, -hz));
    pos.push_back(vec3(-hx, -hy, -hz));

    // Right

    pos.push_back(vec3(-hx, hy, hz));
    pos.push_back(vec3(hx, hy, hz));
    pos.push_back(vec3(hx, hy, -hz));

    pos.push_back(vec3(-hx, hy, hz));
    pos.push_back(vec3(hx, hy, -hz));
    pos.push_back(vec3(-hx, hy, -hz));

    // Left

    pos.push_back(vec3(hx, -hy, hz));
    pos.push_back(vec3(-hx, -hy, hz));
    pos.push_back(vec3(-hx, -hy, -hz));

    pos.push_back(vec3(hx, -hy, hz));
    pos.push_back(vec3(-hx, -hy, -hz));
    pos.push_back(vec3(hx, -hy, -hz));

    // Top

    pos.push_back(vec3(hx, hy, hz));
    pos.push_back(vec3(-hx, hy, hz));
    pos.push_back(vec3(-hx, -hy, hz));

    pos.push_back(vec3(hx, hy, hz));
    pos.push_back(vec3(-hx, -hy, hz));
    pos.push_back(vec3(hx, -hy, hz));

    // Bottom

    pos.push_back(vec3(-hx, hy, -hz));
    pos.push_back(vec3(hx, hy, -hz));
    pos.push_back(vec3(hx, -hy, -hz));

    pos.push_back(vec3(-hx, hy, -hz));
    pos.push_back(vec3(hx, -hy, -hz));
    pos.push_back(vec3(-hx, -hy, -hz));
}

/**
 * GenNormals
 */

void Box::GenNormals(vector<vec3> &norms)
{
    float hx = 0.5;
    float hy = 0.5;
    float hz = 0.5;

    // Front

    norms.push_back(vec3(1.0, 0.0, 0.0));
    norms.push_back(vec3(1.0, 0.0, 0.0));
    norms.push_back(vec3(1.0, 0.0, 0.0));

    norms.push_back(vec3(1.0, 0.0, 0.0));
    norms.push_back(vec3(1.0, 0.0, 0.0));
    norms.push_back(vec3(1.0, 0.0, 0.0));

    // Back

    norms.push_back(vec3(-1.0, 0.0, 0.0));
    norms.push_back(vec3(-1.0, 0.0, 0.0));
    norms.push_back(vec3(-1.0, 0.0, 0.0));

    norms.push_back(vec3(-1.0, 0.0, 0.0));
    norms.push_back(vec3(-1.0, 0.0, 0.0));
    norms.push_back(vec3(-1.0, 0.0, 0.0));

    // Right

    norms.push_back(vec3(0.0, 1.0, 0.0));
    norms.push_back(vec3(0.0, 1.0, 0.0));
    norms.push_back(vec3(0.0, 1.0, 0.0));

    norms.push_back(vec3(0.0, 1.0, 0.0));
    norms.push_back(vec3(0.0, 1.0, 0.0));
    norms.push_back(vec3(0.0, 1.0, 0.0));

    // Left

    norms.push_back(vec3(0.0, -1.0, 0.0));
    norms.push_back(vec3(0.0, -1.0, 0.0));
    norms.push_back(vec3(0.0, -1.0, 0.0));

    norms.push_back(vec3(0.0, -1.0, 0.0));
    norms.push_back(vec3(0.0, -1.0, 0.0));
    norms.push_back(vec3(0.0, -1.0, 0.0));

    // Top

    norms.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0.0, 0.0, 1.0));

    norms.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0.0, 0.0, 1.0));

    // Bottom

    norms.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0.0, 0.0, -1.0));

    norms.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0.0, 0.0, -1.0));
}

/**
 * GenUVs
 */

void Box::GenUVs(vector<vec2> &uvs)
{
    float hx = 0.5;
    float hy = 0.5;
    float hz = 0.5;

    // Front

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

    // Back

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

    // Right

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

    // Left

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

    // Top

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));

    // Bottom

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));

    uvs.push_back(vec2(1.0f, 0.0f));
    uvs.push_back(vec2(0.0f, 1.0f));
    uvs.push_back(vec2(1.0f, 1.0f));
}

/**
 * GenTans
 */

void Box::GenTans(vector<vec3> &tans)
{
    // Front

    tans.push_back(vec3(0.0, 1.0, 0.0));
    tans.push_back(vec3(0.0, 1.0, 0.0));
    tans.push_back(vec3(0.0, 1.0, 0.0));

    tans.push_back(vec3(0.0, 1.0, 0.0));
    tans.push_back(vec3(0.0, 1.0, 0.0));
    tans.push_back(vec3(0.0, 1.0, 0.0));

    // Back

    tans.push_back(vec3(0.0, -1.0, 0.0));
    tans.push_back(vec3(0.0, -1.0, 0.0));
    tans.push_back(vec3(0.0, -1.0, 0.0));

    tans.push_back(vec3(0.0, -1.0, 0.0));
    tans.push_back(vec3(0.0, -1.0, 0.0));
    tans.push_back(vec3(0.0, -1.0, 0.0));

    // Right

    tans.push_back(vec3(-1.0, 0.0, 0.0));
    tans.push_back(vec3(-1.0, 0.0, 0.0));
    tans.push_back(vec3(-1.0, 0.0, 0.0));

    tans.push_back(vec3(-1.0, 0.0, 0.0));
    tans.push_back(vec3(-1.0, 0.0, 0.0));
    tans.push_back(vec3(-1.0, 0.0, 0.0));

    // Left

    tans.push_back(vec3(1.0, 0.0, 0.0));
    tans.push_back(vec3(1.0, 0.0, 0.0));
    tans.push_back(vec3(1.0, 0.0, 0.0));

    tans.push_back(vec3(1.0, 0.0, 0.0));
    tans.push_back(vec3(1.0, 0.0, 0.0));
    tans.push_back(vec3(1.0, 0.0, 0.0));

    // Top

    tans.push_back(vec3(1.0, 0.0, 0.0));
    tans.push_back(vec3(1.0, 0.0, 0.0));
    tans.push_back(vec3(1.0, 0.0, 0.0));

    tans.push_back(vec3(1.0, 0.0, 0.0));
    tans.push_back(vec3(1.0, 0.0, 0.0));
    tans.push_back(vec3(1.0, 0.0, 0.0));

    // Bottom

    tans.push_back(vec3(-1.0, 0.0, 0.0));
    tans.push_back(vec3(-1.0, 0.0, 0.0));
    tans.push_back(vec3(-1.0, 0.0, 0.0));

    tans.push_back(vec3(-1.0, 0.0, 0.0));
    tans.push_back(vec3(-1.0, 0.0, 0.0));
    tans.push_back(vec3(-1.0, 0.0, 0.0));
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

/**
 * Intersect.
 */

float Box::Intersect(vec3 org, vec3 ray)
{
    // Intersect the ray with each box plane. Return immediately
    // if any hit.

    float d = 0.0;

    vec4 p1 = model * vec4(1.0, 0.0, 0.0, 1.0);
    vec3 p = vec3(p1.x, p1.y, p1.z);
    vec3 norm = vec3(1.0, 0.0, 0.0);
    mat4 invModel = inverse(model);

    if (d = IntersectPlane(org, ray, p, norm, invModel, vec3(0.0, 1.0, 1.0)) > 0.0)
    {
        return d;
    }

    p1 = model * vec4(-1.0, 0.0, 0.0, 1.0);
    p = vec3(p1.x, p1.y, p1.z);
    norm = vec3(-1.0, 0.0, 0.0);

    if (d = IntersectPlane(org, ray, p, norm, invModel, vec3(0.0, 1.0, 1.0)) > 0.0)
    {
        return d;
    }

    p1 = model * vec4(0.0, 1.0, 0.0, 1.0);
    p = vec3(p1.x, p1.y, p1.z);
    norm = vec3(0.0, 1.0, 0.0);

    if (d = IntersectPlane(org, ray, p, norm, invModel, vec3(1.0, 0.0, 1.0)) > 0.0)
    {
        return d;
    }

    p1 = model * vec4(0.0, -1.0, 0.0, 1.0);
    p = vec3(p1.x, p1.y, p1.z);
    norm = vec3(0.0, -1.0, 0.0);

    if (d = IntersectPlane(org, ray, p, norm, invModel, vec3(1.0, 0.0, 1.0)) > 0.0)
    {
        return d;
    }

    p1 = model * vec4(0.0, 0.0, 1.0, 1.0);
    p = vec3(p1.x, p1.y, p1.z);
    norm = vec3(0.0, 0.0, 1.0);

    if (d = IntersectPlane(org, ray, p, norm, invModel, vec3(1.0, 1.0, 0.0)) > 0.0)
    {
        return d;
    }

    p1 = model * vec4(0.0, 0.0, -1.0, 1.0);
    p = vec3(p1.x, p1.y, p1.z);
    norm = vec3(0.0, 0.0, -1.0);

    if (d = IntersectPlane(org, ray, p, norm, invModel, vec3(1.0, 1.0, 0.0)) > 0.0)
    {
        return d;
    }

    return -1.0;
}