#include "sphere.h"

/**
 * Create - Create a sphere.
 */

void Sphere::Create(
    uint32_t numSectors,
    uint32_t numRings,
    vec3 color,
    GLuint progID)
{
    shaderProgID = progID;
    clr = color;

    vector<vec3> verts;
    vector<vec3> norms;

    float dtheta = pi<float>() / (numRings + 2);
    float dphi = 2.0f * pi<float>() / numSectors;

    // Geometry for sphere excluding caps as a big triangle strip.

    for (uint32_t i = 0; i < numRings; i++)
    {
        float x = sinf(dtheta * (i + 1));
        float y = 0.0f;
        float z = cosf(dtheta * (i + 1));

        verts.push_back(vec3(x, y, z));
        norms.push_back(normalize(vec3(x, y, z)));

        for (uint32_t j = 0; j < numSectors; j++)
        {
            x = sinf(dtheta * (i + 2)) * cosf(dphi * j);
            y = sinf(dtheta * (i + 2)) * sinf(dphi * j);
            z = cosf(dtheta * (i + 2));

            verts.push_back(vec3(x, y, z));
            norms.push_back(normalize(vec3(x, y, z)));

            x = sinf(dtheta * (i + 1)) * cosf(dphi * (j + 1));
            y = sinf(dtheta * (i + 1)) * sinf(dphi * (j + 1));
            z = cosf(dtheta * (i + 1));

            verts.push_back(vec3(x, y, z));
            norms.push_back(normalize(vec3(x, y, z)));
        }

        x = sinf(dtheta * (i + 2));
        y = 0.0f;
        z = cosf(dtheta * (i + 2));

        verts.push_back(vec3(x, y, z));
        norms.push_back(normalize(vec3(x, y, z)));
    }

    // Caps as a pair of triangle fans.

    numSideVerts = verts.size();
    topOffset = verts.size();

    verts.push_back(vec3(0.0, 0.0, 1.0));
    norms.push_back(vec3(0, 0, 1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = sinf(dtheta) * cosf(i * dphi);
        float y = sinf(dtheta) * sinf(i * dphi);
        float z = cosf(dtheta);

        verts.push_back(vec3(x, y, z));
        norms.push_back(normalize(vec3(x, y, z)));
    }

    bottomOffset = verts.size();
    numCapVerts = bottomOffset - topOffset;

    verts.push_back(vec3(0.0, 0.0, -1.0));
    norms.push_back(vec3(0, 0, -1.0));

    for (uint32_t i = 0; i < numSectors + 1; i++)
    {
        float x = sinf(dtheta) * cosf(i * dphi);
        float y = -sinf(dtheta) * sinf(i * dphi);
        float z = -cosf(dtheta);

        verts.push_back(vec3(x, y, z));
        norms.push_back(normalize(vec3(x, y, z)));
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

    mat4 sc = mat4(1.0f);
    mat4 rot = mat4(1.0);
    mat4 trans = translate(vec3(-3.0, 3.0, 2.0));

    model = trans * rot * sc;
    modelInv = inverseTranspose(model);
}

/**
 * Draw - Draw the sphere. Set it's model matrices and color in the shader.
 * Sphere is drawn in three parts: sides, top cap, and bottom cap.
 */

void Sphere::Draw()
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