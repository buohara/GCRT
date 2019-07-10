#include "box.h"

/**
 * GenPositions Generate vertex positions for a box.
 * @param pos Vector to fill with box positions.
 */

void GenPositionsBox(vector<vec3> &pos)
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
 * GenNormals Get vertex normals for a box.
 * @param norms Vector of normals to fill.
 */

void GenNormalsBox(vector<vec3> &norms)
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
 * GenUVs Generate vertex UVs for a box.
 * @param uvs Vector of UVs to fill.
 */

void GenUVsBox(vector<vec2> &uvs)
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
 * GenTans Generate vertex tangents for a box.
 * @param tans Vector of tangents to fill.
 */

void GenTansBox(vector<vec3> &tans)
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