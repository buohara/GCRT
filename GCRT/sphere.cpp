#include "sphere.h"

/**
 * Create - Create a sphere.
 */

void Sphere::Create(
    uint32_t numSectors,
    uint32_t numRings
)
{
    vector<vec3> verts;
    vector<vec3> norms;
    vector<vec2> uvs;

    float dtheta = pi<float>() / (numRings + 2);
    float dphi = 2.0f * pi<float>() / numSectors;
    float du = 1.0 / numSectors;
    float dv = 1.0 / (numRings + 2);

    // Geometry for sphere excluding caps as a big triangle strip.

    for (uint32_t i = 0; i < numRings; i++)
    {
        float x = sinf(dtheta * (i + 1));
        float y = 0.0f;
        float z = cosf(dtheta * (i + 1));
        float u = 0.0f;
        float v = dv * (i + 1);

        verts.push_back(vec3(x, y, z));
        norms.push_back(normalize(vec3(x, y, z)));
        uvs.push_back(vec2(u, v));

        for (uint32_t j = 0; j < numSectors; j++)
        {
            x = sinf(dtheta * (i + 2)) * cosf(dphi * j);
            y = sinf(dtheta * (i + 2)) * sinf(dphi * j);
            z = cosf(dtheta * (i + 2));
            u = du * j;
            v = dv * (i + 2);

            verts.push_back(vec3(x, y, z));
            norms.push_back(normalize(vec3(x, y, z)));
            uvs.push_back(vec2(u, v));

            x = sinf(dtheta * (i + 1)) * cosf(dphi * (j + 1));
            y = sinf(dtheta * (i + 1)) * sinf(dphi * (j + 1));
            z = cosf(dtheta * (i + 1));
            u = du * (j + 1);
            v = dv * (i + 1);

            verts.push_back(vec3(x, y, z));
            norms.push_back(normalize(vec3(x, y, z)));
            uvs.push_back(vec2(u, v));
        }

        x = sinf(dtheta * (i + 2));
        y = 0.0f;
        z = cosf(dtheta * (i + 2));
        u = 1.0;
        v = dv * (i + 2);

        verts.push_back(vec3(x, y, z));
        norms.push_back(normalize(vec3(x, y, z)));
        uvs.push_back(vec2(u, v));
    }

    // Caps as a pair of triangle fans.

    numSideVerts = verts.size();
    topOffset = verts.size();

    verts.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0, 0, 1.0));
    uvs.push_back(vec2(0.5, 0.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = sinf(dtheta) * cosf(i * dphi);
        float y = sinf(dtheta) * sinf(i * dphi);
        float z = cosf(dtheta);
        float u = i * du;
        float v = dv;

        verts.push_back(vec3(x, y, z));
        norms.push_back(normalize(vec3(x, y, z)));
        uvs.push_back(vec2(u, v));
    }

    bottomOffset = verts.size();
    numCapVerts = bottomOffset - topOffset;

    verts.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0, 0, -1.0));
    uvs.push_back(vec2(0.5, 1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = sinf(dtheta) * cosf(i * dphi);
        float y = -sinf(dtheta) * sinf(i * dphi);
        float z = -cosf(dtheta);
        float u = i * du;
        float v = 1.0 - dv;

        verts.push_back(vec3(x, y, z));
        norms.push_back(normalize(vec3(x, y, z)));
        uvs.push_back(vec2(u, v));
    }

    InitVertexObjects(verts, norms, uvs);
    InitModelMatrices();
}

/**
 * Draw - Draw the sphere. Set it's model matrices and color in the shader.
 * Sphere is drawn in three parts: sides, top cap, and bottom cap.
 */

void Sphere::Draw()
{
    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numSideVerts);
    glDrawArrays(GL_TRIANGLE_FAN, topOffset, numCapVerts);
    glDrawArrays(GL_TRIANGLE_FAN, bottomOffset, numCapVerts);
    glBindVertexArray(0);
}