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
    vec3 color,
    GLuint progID)
{
    shaderProgID = progID;
    clr = color;

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
    size_t vertBufSize = 3 * verts.size() * sizeof(float);
    size_t normBufSize = 3 * norms.size() * sizeof(float);

    // Create buffer objects and upload to GPU.

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &vertVboID);
    glBindBuffer(GL_ARRAY_BUFFER, vertVboID);
    glBufferData(GL_ARRAY_BUFFER, vertBufSize, &verts[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &normVboID);
    glBindBuffer(GL_ARRAY_BUFFER, normVboID);
    glBufferData(GL_ARRAY_BUFFER, normBufSize, &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Create model matrices.

    mat4 sc = scale(mat4(1.0f), vec3(10.0f, 10.0f, 1.0f));
    mat4 trans = translate(vec3(0.0, 0.0, -1.0f));

    model = trans * sc;
    modelInv = inverseTranspose(model);
}

/**
 * Draw
 */

void Plane::Draw()
{
    GLuint modelID = glGetUniformLocation(shaderProgID, "model");
    glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

    GLuint modelInvID = glGetUniformLocation(shaderProgID, "modelInv");
    glUniformMatrix4fv(modelInvID, 1, false, &modelInv[0][0]);

    GLuint kdID = glGetUniformLocation(shaderProgID, "kd");
    glUniform3fv(kdID, 1, &clr[0]);

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerts);
    glBindVertexArray(0);
}