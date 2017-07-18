#include "sphere.h"

/**
 * [Sphere::Create description]
 * @param numSectors [description]
 * @param numRings   [description]
 */

void Sphere::Create(
    uint32_t numSectors,
    uint32_t numRings
)
{
    vector<vec3> pos;
    vector<vec3> norms;
    vector<vec2> uvs;
    vector<vec3> tans;
    vector<ivec4> boneIDs;
    vector<vec4> boneWts;

    animated = false;

    GenPositions(pos, numSectors, numRings);
    GenNormals(norms, numSectors, numRings);
    GenUVs(uvs, numSectors, numRings);
    GenTans(tans, numSectors, numRings);

    boneIDs.resize(pos.size(), ivec4(0));
    boneWts.resize(pos.size(), vec4(1.0f, 0.0f, 0.0f, 0.0f));

    subMeshes.resize(1);
    InitVertexObjects(0, pos, norms, uvs, tans, boneIDs, boneWts);
}

/**
 * [Sphere::GenPositions description]
 * @param pos        [description]
 * @param numSectors [description]
 * @param numRings   [description]
 */

void Sphere::GenPositions(vector<vec3> &pos, uint32_t numSectors, uint32_t numRings)
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
 * [Sphere::GenNormals description]
 * @param norms      [description]
 * @param numSectors [description]
 * @param numRings   [description]
 */

void Sphere::GenNormals(vector<vec3> &norms, uint32_t numSectors, uint32_t numRings)
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
 * [Sphere::GenUVs description]
 * @param uvs        [description]
 * @param numSectors [description]
 * @param numRings   [description]
 */

void Sphere::GenUVs(vector<vec2> &uvs, uint32_t numSectors, uint32_t numRings)
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
 * [Sphere::GenTans description]
 * @param tans       [description]
 * @param numSectors [description]
 * @param numRings   [description]
 */

void Sphere::GenTans(vector<vec3> &tans, uint32_t numSectors, uint32_t numRings)
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

/**
 * [Sphere::Draw description]
 */

void Sphere::Draw()
{
    glBindVertexArray(subMeshes[0].vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numSideVerts);
    glDrawArrays(GL_TRIANGLE_FAN, topOffset, numCapVerts);
    glDrawArrays(GL_TRIANGLE_FAN, bottomOffset, numCapVerts);
    glBindVertexArray(0);
}