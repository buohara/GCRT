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
    uint32_t cols,
    GLuint progID)
{
    shaderProgID = progID;
    kd = vec3(0.5, 0.5, 0.5);

    // Generate a grid of vertices/normals.

    vector<vec3> verts;
    vector<vec3> norms;

    float xstride = 2.0f / (float)cols;
    float ystride = 2.0f / (float)rows;

    for (uint32_t i = 0; i < rows; i++)
    {
        float x = -1.0f;
        float y = 1.0f - ystride * i;
        float z = 0.0f;
        
        verts.push_back({ x, y, z });
        norms.push_back({ 0.0, 0.0, 1.0 });

        for (uint32_t j = 0; j < cols; j++)
        {
            x = -1.0f + j * xstride;
            y = 1.0f - ystride * (i + 1);
            z = 0.0f;
            
            verts.push_back({ x, y, z });
            norms.push_back({ 0.0, 0.0, 1.0 });

            x = -1.0f + (j + 1) * xstride;
            y = 1.0f - ystride * i;
            z = 0.0f;
            
            verts.push_back({ x, y, z });
            norms.push_back({ 0.0, 0.0, 1.0 });
        }

        x = 1.0f;
        y = 1.0f - ystride * (i + 1);
        z = 0.0f;
        
        verts.push_back({ x, y, z });
        norms.push_back({ 0.0, 0.0, 1.0 });
    }

    numVerts = verts.size();

    InitVertexObjects(verts, norms);
    InitModelMatrices();
}

/**
 * Draw
 */

void Plane::Draw()
{
    SetUniforms();

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerts);
    glBindVertexArray(0);
}