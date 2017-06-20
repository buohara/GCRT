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
    vector<ivec4> boneIDs;
    vector<vec4> boneWts;

    animated = false;

    GenPositions(pos);
    GenNormals(norms);
    GenUVs(uvs);
    GenTans(tans);

    boneIDs.resize(pos.size(), ivec4(0));
    boneWts.resize(pos.size(), vec4(1.0f, 0.0f, 0.0f, 0.0f));

    numVerts = (uint32_t)pos.size();

    subMeshes.resize(1);
    InitVertexObjects(0, pos, norms, uvs, tans, boneIDs, boneWts);
}

/**
 * [Box::GenPositions description]
 * @param pos [description]
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
 * [Box::GenNormals description]
 * @param norms [description]
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
 * [Box::GenUVs description]
 * @param uvs [description]
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
 * [Box::GenTans description]
 * @param tans [description]
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
 * [Box::Draw description]
 */

void Box::Draw()
{
    glBindVertexArray(subMeshes[0].vaoID);
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);
}

/**
 * [Box::SetBoneMatrices description]
 * @param renderProgram [description]
 */

void Box::SetBoneMatrices(GLuint renderProgram)
{
    GLuint bonesID = glGetUniformLocation(renderProgram, "bones");
    glUniformMatrix4fv(bonesID, 1, false, &model[0][0]);
}