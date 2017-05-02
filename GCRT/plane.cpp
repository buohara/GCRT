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

    vector<vec3> verts;
    vector<vec3> norms;
    vector<vec2> uvs;

    float xstride = 2.0f / (float)cols;
    float ystride = 2.0f / (float)rows;
    float ustride = 1.0f / (float)cols;
    float vstride = 1.0f / (float)rows;

    for (uint32_t i = 0; i < rows; i++)
    {
        // Top left corner of each row

        float x = -1.0f;
        float y = 1.0f - ystride * i;
        float z = 0.0f;
        float u = 0.0f;
        float v = i * vstride;
        
        verts.push_back({ x, y, z });
        norms.push_back({ 0.0, 0.0, 1.0 });
        uvs.push_back({ u, v });

        // Zig-zag down the row making a triangle strip.

        for (uint32_t j = 0; j < cols; j++)
        {
            x = -1.0f + j * xstride;
            y = 1.0f - ystride * (i + 1);
            z = 0.0f;
            u = j * ustride;
            v = (i + 1) * vstride;
            
            verts.push_back({ x, y, z });
            norms.push_back({ 0.0, 0.0, 1.0 });
            uvs.push_back({ u, v });

            x = -1.0f + (j + 1) * xstride;
            y = 1.0f - ystride * i;
            z = 0.0f;
            u = (j + 1) * ustride;
            v = i * vstride;

            verts.push_back({ x, y, z });
            norms.push_back({ 0.0, 0.0, 1.0 });
            uvs.push_back({ u, v });
        }

        // Bottom right corner of the row

        x = 1.0f;
        y = 1.0f - ystride * (i + 1);
        z = 0.0f;
        u = 1.0f;
        v = (i + 1) * vstride;
        
        verts.push_back({ x, y, z });
        norms.push_back({ 0.0, 0.0, 1.0 });
        uvs.push_back({ u, v });
    }

    numVerts = verts.size();
    InitVertexObjects(verts, norms, uvs);
    InitModelMatrices();
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