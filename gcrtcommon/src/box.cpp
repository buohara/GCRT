#include "box.h"

/**
 * Box::Create - Create a box.
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
 * Box::CreateVk Create a box using Vulkan.
 *
 * @param logicalDevice          Vk device to create box [in].
 * @param cmdBuf                 Command buffer to record mesh uploads into [in].
 * @param deviceMemoryProperties Available memory heaps for allocating mesh geometry [in].
 */

void Box::CreateVk(
    VkDevice &logicalDevice,
    VkCommandBuffer &cmdBuf,
    VkPhysicalDeviceMemoryProperties &deviceMemoryProperties
)
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
    
    InitVertexObjectsVk(
        0, 
        pos, 
        norms, 
        uvs, 
        tans, 
        boneIDs, 
        boneWts,
        logicalDevice,
        deviceMemoryProperties,
        cmdBuf
    );
}

/**
 * Box::GenPositions Generate vertex positions for a box.
 * @param pos Vector to fill with box positions.
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
 * Box::GenNormals Get vertex normals for a box.
 * @param norms Vector of normals to fill.
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
 * Box::GenUVs Generate vertex UVs for a box.
 * @param uvs Vector of UVs to fill.
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
 * Box::GenTans Generate vertex tangents for a box.
 * @param tans Vector of tangents to fill.
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
 * Box::Draw Set box VAO and do draw call.
 */

void Box::Draw()
{
    glBindVertexArray(subMeshes[0].vaoID);
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);
}