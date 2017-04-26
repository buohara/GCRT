#include "cylinder.h"

/**
 * Create - Create a cylinder.
 */

void Cylinder::Create(
    uint32_t numSectors,
    vec3 color,
    GLuint progID)
{
    shaderProgID = progID;
    clr = color;

    vector<vec3> verts;
    vector<vec3> norms;

    // Verts for the sides.

    float dtheta = 2 * glm::pi<float>() / numSectors;
    float hz = 0.5;

    for (uint32_t i = 0; i < numSectors; i++)
    {
        float x1 = cosf(i * dtheta);
        float x2 = cosf((i + 1) * dtheta);
        float y1 = sinf(i * dtheta);
        float y2 = sinf((i + 1) * dtheta);

        verts.push_back(vec3(x1, y1, hz));
        verts.push_back(vec3(x1, y1, -hz));
        verts.push_back(vec3(x2, y2, hz));
        verts.push_back(vec3(x2, y2, -hz));

        norms.push_back(normalize(vec3(x1, y1, 0)));
        norms.push_back(normalize(vec3(x1, y1, 0)));
        norms.push_back(normalize(vec3(x2, y2, 0)));
        norms.push_back(normalize(vec3(x2, y2, 0)));
    }

    // Verts for top and bottom;

    topOffset = verts.size();
    numSideVerts = verts.size();

    verts.push_back(vec3(0.0, 0.0, hz));
    norms.push_back(vec3(0, 0, 1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = cosf(i * dtheta);
        float y = sinf(i * dtheta);
        verts.push_back(vec3(x, y, hz));
        norms.push_back(vec3(0, 0, 1.0));
    }

    bottomOffset = verts.size();
    numCapVerts = verts.size() - topOffset;

    verts.push_back(vec3(0.0, 0.0, -hz));
    norms.push_back(vec3(0, 0, -1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = cosf(i * dtheta);
        float y = -sinf(i * dtheta);
        verts.push_back(vec3(x, y, -hz));
        norms.push_back(vec3(0, 0, -1.0));
    }

    numVerts = verts.size();

    // Create GPU vertex buffers and upload data.

    size_t vertBufSize = 3 * numVerts * sizeof(float);
    size_t normBufSize = 3 * numVerts * sizeof(float);

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

    // Initialize model matrices.

    mat4 sc = scale(mat4(1.0), vec3(1.0, 1.0, 5.0));
    mat4 rot = mat4(1.0);
    mat4 trans = translate(vec3(-3.0, -3.0, 0.0));

    model = trans * rot * sc;
    modelInv = inverseTranspose(model);
}

/**
 * Draw - Draw the cylinder. Set it's model matrices and color in the shader.
 * Cylinder is drawn in three parts: sides, top, and bottom.
 */

void Cylinder::Draw()
{
    GLuint modelID = glGetUniformLocation(shaderProgID, "model");
    glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

    GLuint modelInvID = glGetUniformLocation(shaderProgID, "modelInv");
    glUniformMatrix4fv(modelInvID, 1, false, &modelInv[0][0]);

    GLuint kdID = glGetUniformLocation(shaderProgID, "kd");
    glUniform3fv(kdID, 1, &clr[0]);

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numSideVerts);
    glDrawArrays(GL_TRIANGLE_FAN, topOffset, numCapVerts);
    glDrawArrays(GL_TRIANGLE_FAN, bottomOffset, numCapVerts);
    glBindVertexArray(0);
}