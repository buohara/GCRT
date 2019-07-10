#include "cylinder.h"

/**
 * GenPositions Generate cylinder vertex positions.
 *
 * @param pos        Vector of vertex positions to fill.
 * @param numSectors Number of cylinder sectors.
 */

void GenPositionsCylinder(
    vector<vec3> &pos,
    uint32_t numSectors,
    uint32_t &numSideVerts,
    uint32_t &topOffset,
    uint32_t &bottomOffset,
    uint32_t &numCapVerts
)
{
    // Verts for the sides.

    float dtheta    = 2 * glm::pi<float>() / numSectors;
    float hz        = 0.5;

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

    topOffset       = (uint32_t)pos.size();
    numSideVerts    = (uint32_t)pos.size();

    pos.push_back(vec3(0.0, 0.0, hz));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = cosf(i * dtheta);
        float y = sinf(i * dtheta);
        pos.push_back(vec3(x, y, hz));
    }

    bottomOffset    = (uint32_t)pos.size();
    numCapVerts     = (uint32_t)(pos.size() - topOffset);

    pos.push_back(vec3(0.0, 0.0, -hz));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = cosf(i * dtheta);
        float y = -sinf(i * dtheta);
        pos.push_back(vec3(x, y, -hz));
    }
}

/**
 * GenNormals Generate cylinder vertex normals.
 * @param norms      Vector of normals to fill out.
 * @param numSectors Number of cylinder sectors.
 */

void GenNormalsCylinder(vector<vec3> &norms, uint32_t numSectors)
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
 * GenUVs Generate cylinder vertex UVs.
 * @param uvs        Vector of UVs to fill.
 * @param numSectors Number of cylinder sectors.
 */

void GenUVsCylinder(vector<vec2> &uvs, uint32_t numSectors)
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
 * GenTans Generate vertex tangents.
 * @param tans       Vector of tangets to fill.
 * @param numSectors Number of cylinder sectors.
 */

void GenTansCylinder(vector<vec3> &tans, uint32_t numSectors)
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