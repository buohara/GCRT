#include "sphere.h"

/**
 * [GenPositions description]
 * @param pos        [description]
 * @param numSectors [description]
 * @param numRings   [description]
 */

void GenPositionsSphere(
    vector<vec3> &pos,
    uint32_t numSectors,
    uint32_t numRings,
    uint32_t& numSideVerts,
    uint32_t& topOffset,
    uint32_t& bottomOffset,
    uint32_t& numCapVerts
)
{
    float dtheta = pi<float>() / (numRings + 2);
    float dphi = 2.0f * pi<float>() / numSectors;

    // Geometry for sphere excluding caps as a big triangle strip.

    for (uint32_t i = 0; i < numRings; i++)
    {
        float x = sinf(dtheta * (i + 1));
        float y = 0.0f;
        float z = cosf(dtheta * (i + 1));

        pos.push_back(vec3(x, y, z));

        for (uint32_t j = 0; j < numSectors; j++)
        {
            x = sinf(dtheta * (i + 2)) * cosf(dphi * j);
            y = sinf(dtheta * (i + 2)) * sinf(dphi * j);
            z = cosf(dtheta * (i + 2));

            pos.push_back(vec3(x, y, z));

            x = sinf(dtheta * (i + 1)) * cosf(dphi * (j + 1));
            y = sinf(dtheta * (i + 1)) * sinf(dphi * (j + 1));
            z = cosf(dtheta * (i + 1));

            pos.push_back(vec3(x, y, z));
        }

        x = sinf(dtheta * (i + 2));
        y = 0.0f;
        z = cosf(dtheta * (i + 2));

        pos.push_back(vec3(x, y, z));
    }

    // Caps as a pair of triangle fans.

    numSideVerts = (uint32_t)pos.size();
    topOffset = (uint32_t)pos.size();

    pos.push_back(vec3(0.0, 0.0, 1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = sinf(dtheta) * cosf(i * dphi);
        float y = sinf(dtheta) * sinf(i * dphi);
        float z = cosf(dtheta);

        pos.push_back(vec3(x, y, z));
    }

    bottomOffset = (uint32_t)pos.size();
    numCapVerts = bottomOffset - topOffset;

    pos.push_back(vec3(0.0, 0.0, -1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = sinf(dtheta) * cosf(i * dphi);
        float y = -sinf(dtheta) * sinf(i * dphi);
        float z = -cosf(dtheta);

        pos.push_back(vec3(x, y, z));
    }
}

/**
 * [GenNormals description]
 * @param norms      [description]
 * @param numSectors [description]
 * @param numRings   [description]
 */

void GenNormalsSphere(vector<vec3> &norms, uint32_t numSectors, uint32_t numRings)
{
    float dtheta = pi<float>() / (numRings + 2);
    float dphi = 2.0f * pi<float>() / numSectors;

    // Geometry for sphere excluding caps as a big triangle strip.

    for (uint32_t i = 0; i < numRings; i++)
    {
        float x = sinf(dtheta * (i + 1));
        float y = 0.0f;
        float z = cosf(dtheta * (i + 1));

        norms.push_back(normalize(vec3(x, y, z)));

        for (uint32_t j = 0; j < numSectors; j++)
        {
            x = sinf(dtheta * (i + 2)) * cosf(dphi * j);
            y = sinf(dtheta * (i + 2)) * sinf(dphi * j);
            z = cosf(dtheta * (i + 2));
            norms.push_back(normalize(vec3(x, y, z)));

            x = sinf(dtheta * (i + 1)) * cosf(dphi * (j + 1));
            y = sinf(dtheta * (i + 1)) * sinf(dphi * (j + 1));
            z = cosf(dtheta * (i + 1));
            norms.push_back(normalize(vec3(x, y, z)));
        }

        x = sinf(dtheta * (i + 2));
        y = 0.0f;
        z = cosf(dtheta * (i + 2));
        norms.push_back(normalize(vec3(x, y, z)));
    }

    // Caps as a pair of triangle fans.

    norms.push_back(vec3(0, 0, 1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = sinf(dtheta) * cosf(i * dphi);
        float y = sinf(dtheta) * sinf(i * dphi);
        float z = cosf(dtheta);
        norms.push_back(normalize(vec3(x, y, z)));
    }

    norms.push_back(vec3(0, 0, -1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = sinf(dtheta) * cosf(i * dphi);
        float y = -sinf(dtheta) * sinf(i * dphi);
        float z = -cosf(dtheta);
        norms.push_back(normalize(vec3(x, y, z)));
    }
}

/**
 * [GenUVs description]
 * @param uvs        [description]
 * @param numSectors [description]
 * @param numRings   [description]
 */

void GenUVsSphere(vector<vec2> &uvs, uint32_t numSectors, uint32_t numRings)
{
    float du = 1.0f / (float)numSectors;
    float dv = 1.0f / (float)(numRings + 2);

    // Geometry for sphere excluding caps as a big triangle strip.

    for (uint32_t i = 0; i < numRings; i++)
    {
        float u = 0.0f;
        float v = dv * (i + 1);

        uvs.push_back(vec2(u, v));

        for (uint32_t j = 0; j < numSectors; j++)
        {
            u = du * j;
            v = dv * (i + 2);
            uvs.push_back(vec2(u, v));

            u = du * (j + 1);
            v = dv * (i + 1);
            uvs.push_back(vec2(u, v));
        }

        u = 1.0;
        v = dv * (i + 2);

        uvs.push_back(vec2(u, v));
    }

    // Caps as a pair of triangle fans.

    uvs.push_back(vec2(0.5, 0.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float u = i * du;
        float v = dv;
        uvs.push_back(vec2(u, v));
    }

    uvs.push_back(vec2(0.5, 1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float u = i * du;
        float v = 1.0f - dv;

        uvs.push_back(vec2(u, v));
    }
}

/**
 * [GenTans description]
 * @param tans       [description]
 * @param numSectors [description]
 * @param numRings   [description]
 */

void GenTansSphere(vector<vec3> &tans, uint32_t numSectors, uint32_t numRings)
{
    float dtheta = pi<float>() / (numRings + 2);
    float dphi = 2.0f * pi<float>() / numSectors;

    // Geometry for sphere excluding caps as a big triangle strip.

    for (uint32_t i = 0; i < numRings; i++)
    {
        float x = 0.0f;
        float y = sinf(dtheta * (i + 1));
        float z = 0.0f;
        tans.push_back(normalize(vec3(x, y, z)));

        for (uint32_t j = 0; j < numSectors; j++)
        {
            x = -sinf(dtheta * (i + 2)) * sinf(dphi * j);
            y = sinf(dtheta * (i + 2)) * cosf(dphi * j);
            z = 0.0f;
            tans.push_back(normalize(vec3(x, y, z)));

            x = -sinf(dtheta * (i + 1)) * sinf(dphi * (j + 1));
            y = sinf(dtheta * (i + 1)) * cosf(dphi * (j + 1));
            z = 0.0f;
            tans.push_back(normalize(vec3(x, y, z)));
        }

        x = 0.0f;
        y = sinf(dtheta * (i + 2));
        z = 0.0f;
        tans.push_back(normalize(vec3(x, y, z)));
    }

    // Caps as a pair of triangle fans.

    tans.push_back(vec3(0.0, 1.0, 0.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = -sinf(dtheta) * sinf(i * dphi);
        float y = sinf(dtheta) * cosf(i * dphi);
        float z = 0.0f;
        tans.push_back(normalize(vec3(x, y, z)));
    }

    tans.push_back(vec3(0.0, 1.0, 0.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = -sinf(dtheta) * sinf(i * dphi);
        float y = -sinf(dtheta) * cosf(i * dphi);
        float z = 0.0f;
        tans.push_back(normalize(vec3(x, y, z)));
    }
}