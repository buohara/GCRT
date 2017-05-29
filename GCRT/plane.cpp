#include "plane.h"

/**
 * Create
 *
 * @param rows
 * @param cols
 * @param color
 * prodID
 */

void Plane::Create(
    uint32_t rows,
    uint32_t cols
)
{
    // Generate a grid of vertices/normals.

    vector<vec3> pos;
    vector<vec3> norms;
    vector<vec2> uvs;
    vector<vec3> tans;

    GenPositions(pos, rows, cols);
    GenNormals(norms, rows, cols);
    GenUVs(uvs, rows, cols);
    GenTangents(tans, rows, cols);

    numVerts = (uint32_t)pos.size();
    InitVertexObjects(pos, norms, uvs);
    InitModelMatrices();
}

/**
 * GenPositions
 */

void Plane::GenPositions(vector<vec3> &pos, uint32_t rows, uint32_t cols)
{
    float xstride = 2.0f / (float)cols;
    float ystride = 2.0f / (float)rows;

    for (uint32_t i = 0; i < rows; i++)
    {
        // Top left corner of each row

        float x = -1.0f;
        float y = 1.0f - ystride * i;
        float z = 0.0f;

        pos.push_back({ x, y, z });

        // Zig-zag down the row making a triangle strip.

        for (uint32_t j = 0; j < cols; j++)
        {
            x = -1.0f + j * xstride;
            y = 1.0f - ystride * (i + 1);
            z = 0.0f;

            pos.push_back({ x, y, z });

            x = -1.0f + (j + 1) * xstride;
            y = 1.0f - ystride * i;
            z = 0.0f;

            pos.push_back({ x, y, z });
        }

        // Bottom right corner of the row

        x = 1.0f;
        y = 1.0f - ystride * (i + 1);
        z = 0.0f;

        pos.push_back({ x, y, z });
    }
}

/**
 * GenNormals
 */

void Plane::GenNormals(vector<vec3> &norms, uint32_t rows, uint32_t cols)
{
    for (uint32_t i = 0; i < rows; i++)
    {
        // Top left corner of each row

        norms.push_back({ 0.0, 0.0, 1.0 });

        // Zig-zag down the row making a triangle strip.

        for (uint32_t j = 0; j < cols; j++)
        {
            norms.push_back({ 0.0, 0.0, 1.0 });
            norms.push_back({ 0.0, 0.0, 1.0 });
        }

        // Bottom right corner of the row

        norms.push_back({ 0.0, 0.0, 1.0 });
    }
}

/**
 * GenUVs
 */

void Plane::GenUVs(vector<vec2> &uvs, uint32_t rows, uint32_t cols)
{
    float ustride = 1.0f / (float)cols;
    float vstride = 1.0f / (float)rows;

    for (uint32_t i = 0; i < rows; i++)
    {
        // Top left corner of each row

        float u = 0.0f;
        float v = i * vstride;

        uvs.push_back({ u, v });

        // Zig-zag down the row making a triangle strip.

        for (uint32_t j = 0; j < cols; j++)
        {
            u = j * ustride;
            v = (i + 1) * vstride;

            uvs.push_back({ u, v });

            u = (j + 1) * ustride;
            v = i * vstride;

            uvs.push_back({ u, v });
        }

        // Bottom right corner of the row

        u = 1.0f;
        v = (i + 1) * vstride;

        uvs.push_back({ u, v });
    }
}

/**
 * GenTangents
 */

void Plane::GenTangents(vector<vec3> &tans, uint32_t rows, uint32_t cols)
{
    for (uint32_t i = 0; i < rows; i++)
    {
        tans.push_back(vec3(0.0, 1.0, 0.0));

        // Zig-zag down the row making a triangle strip.

        for (uint32_t j = 0; j < cols; j++)
        {
            tans.push_back(vec3(1.0, 0.0, 0.0));
            tans.push_back(vec3(1.0, 0.0, 0.0));
        }

        // Bottom right corner of the row

        tans.push_back(vec3(0.0, 1.0, 0.0));
    }
}

/**
 * Draw
 */

void Plane::Draw()
{
    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerts);
    glBindVertexArray(0);
}

/**
 * Intersect.
 */

float Plane::Intersect(vec3 org, vec3 ray)
{
    vec4 p1 = model * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 p = vec3(p1.x, p1.y, p1.z);
    vec3 norm = vec3(0.0, 0.0, 1.0);
    mat4 invModel = inverse(model);

    return IntersectPlane(org, ray, p, norm, invModel, vec3(1.0, 1.0, 0.0));
}