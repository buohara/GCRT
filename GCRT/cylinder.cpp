#include "cylinder.h"

/**
 * Create - Create a cylinder.
 */

void Cylinder::Create(
    uint32_t numSectors,
    GLuint progID)
{
    shaderProgID = progID;
    kd = vec3(0.5, 0.5, 0.5);

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

    InitVertexObjects(verts, norms);
    InitModelMatrices();
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
    glUniform3fv(kdID, 1, &kd[0]);

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numSideVerts);
    glDrawArrays(GL_TRIANGLE_FAN, topOffset, numCapVerts);
    glDrawArrays(GL_TRIANGLE_FAN, bottomOffset, numCapVerts);
    glBindVertexArray(0);
}