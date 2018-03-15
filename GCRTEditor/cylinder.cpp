#include "cylinder.h"

/**
 * Cylinder::Create Initialize a cylinder mesh and related GL resources.
 * @param numSectors Number of cylinder sectors.
 */

void Cylinder::Create(
    uint32_t numSectors
)
{
    vector<vec3> pos;
    vector<vec3> norms;
    vector<vec2> uvs;
    vector<vec3> tans;
    vector<ivec4> boneIDs;
    vector<vec4> boneWts;

    animated = false;

    GenPositions(pos, numSectors);
    GenNormals(norms, numSectors);
    GenUVs(uvs, numSectors);
    GenTans(tans, numSectors);

    boneIDs.resize(pos.size(), ivec4(0));
    boneWts.resize(pos.size(), vec4(1.0f, 0.0f, 0.0f, 0.0f));

    subMeshes.resize(1);
    InitVertexObjects(0, pos, norms, uvs, tans, boneIDs, boneWts);
}

/**
 * Cylinder::GenPositions Generate cylinder vertex positions.
 *
 * @param pos        Vector of vertex positions to fill.
 * @param numSectors Number of cylinder sectors.
 */

void Cylinder::GenPositions(vector<vec3> &pos, uint32_t numSectors)
{
    // Verts for the sides.

    float dtheta = 2 * glm::pi<float>() / numSectors;
    float hz = 0.5;

    for (uint32_t i = 0; i < numSectors; i++)
    {
        float x1 = cosf(i * dtheta);
        float x2 = cosf((i + 1) * dtheta);
        float y1 = sinf(i * dtheta);
        float y2 = sinf((i + 1) * dtheta);

        pos.push_back(vec3(x1, y1, hz));
        pos.push_back(vec3(x1, y1, -hz));
        pos.push_back(vec3(x2, y2, hz));
        pos.push_back(vec3(x2, y2, -hz));
    }

    // Verts for top and bottom;

    topOffset = (uint32_t)pos.size();
    numSideVerts = (uint32_t)pos.size();

    pos.push_back(vec3(0.0, 0.0, hz));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = cosf(i * dtheta);
        float y = sinf(i * dtheta);
        pos.push_back(vec3(x, y, hz));
    }

    bottomOffset = (uint32_t)pos.size();
    numCapVerts = (uint32_t)(pos.size() - topOffset);

    pos.push_back(vec3(0.0, 0.0, -hz));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = cosf(i * dtheta);
        float y = -sinf(i * dtheta);

        pos.push_back(vec3(x, y, -hz));
    }
}

/**
 * Cylinder::GenNormals Generate cylinder vertex normals.
 * @param norms      Vector of normals to fill out.
 * @param numSectors Number of cylinder sectors.
 */

void Cylinder::GenNormals(vector<vec3> &norms, uint32_t numSectors)
{
    // Verts for the sides.

    float dtheta = 2 * glm::pi<float>() / numSectors;

    for (uint32_t i = 0; i < numSectors; i++)
    {
        float x1 = cosf(i * dtheta);
        float x2 = cosf((i + 1) * dtheta);
        float y1 = sinf(i * dtheta);
        float y2 = sinf((i + 1) * dtheta);

        norms.push_back(normalize(vec3(x1, y1, 0)));
        norms.push_back(normalize(vec3(x1, y1, 0)));
        norms.push_back(normalize(vec3(x2, y2, 0)));
        norms.push_back(normalize(vec3(x2, y2, 0)));
    }

    // Verts for top and bottom;

    norms.push_back(vec3(0, 0, 1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        norms.push_back(vec3(0, 0, 1.0));
    }

    norms.push_back(vec3(0, 0, -1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        norms.push_back(vec3(0, 0, -1.0));
    }
}

/**
 * Cylinder::GenUVs Generate cylinder vertex UVs.
 * @param uvs        Vector of UVs to fill.
 * @param numSectors Number of cylinder sectors.
 */

void Cylinder::GenUVs(vector<vec2> &uvs, uint32_t numSectors)
{
    // Verts for the sides.

    float du = 1.0f / numSectors;

    for (uint32_t i = 0; i < numSectors; i++)
    {
        float u1 = du * i;
        float u2 = du * (i + 1);
        float v1 = 0.0;
        float v2 = 1.0;

        uvs.push_back(vec2(u1, v1));
        uvs.push_back(vec2(u1, v2));
        uvs.push_back(vec2(u2, v1));
        uvs.push_back(vec2(u2, v2));
    }

    // Verts for top and bottom;

    uvs.push_back(vec2(0.5, 0.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        uvs.push_back(vec2(0.5, 0.0));
    }

    uvs.push_back(vec2(0.5, 1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        uvs.push_back(vec2(0.5, 1.0));
    }
}

/**
 * Cylinder::GenTans Generate vertex tangents.
 * @param tans       Vector of tangets to fill.
 * @param numSectors Number of cylinder sectors.
 */

void Cylinder::GenTans(vector<vec3> &tans, uint32_t numSectors)
{
    // Verts for the sides.

    float dtheta = 2 * glm::pi<float>() / numSectors;

    for (uint32_t i = 0; i < numSectors; i++)
    {
        float x1 = -sinf(i * dtheta);
        float x2 = -sinf((i + 1) * dtheta);
        float y1 = cosf(i * dtheta);
        float y2 = cosf((i + 1) * dtheta);

        tans.push_back(normalize(vec3(x1, y1, 0)));
        tans.push_back(normalize(vec3(x1, y1, 0)));
        tans.push_back(normalize(vec3(x2, y2, 0)));
        tans.push_back(normalize(vec3(x2, y2, 0)));
    }

    // Verts for top and bottom;

    tans.push_back(vec3(0, 1.0, 0.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {

        tans.push_back(vec3(0, 0, 1.0));
    }

    tans.push_back(vec3(0, 1.0, 0.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        tans.push_back(vec3(0, 0, -1.0));
    }
}

/**
 * Cylinder::Draw Set cylinder VAO and draw.
 */

void Cylinder::Draw()
{
    glBindVertexArray(subMeshes[0].vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numSideVerts);
    glDrawArrays(GL_TRIANGLE_FAN, topOffset, numCapVerts);
    glDrawArrays(GL_TRIANGLE_FAN, bottomOffset, numCapVerts);
    glBindVertexArray(0);
}