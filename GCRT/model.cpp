#include "model.h"

/**
 * SetModelMatrices -
 */

void Model::SetModelMatrices(GLuint program)
{
    GLuint modelID = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

    GLuint modelInvID = glGetUniformLocation(program, "modelInv");
    glUniformMatrix4fv(modelInvID, 1, false, &modelInv[0][0]);
}

/**
 * InitVertexObjects -
 */

void Model::InitModelMatrices()
{
    scl = mat4(1.0f);
    rot = mat4(1.0f);
    trans = mat4(1.0f);

    model = trans * scl * rot;
    modelInv = inverseTranspose(model);
}

/**
 * Translate -
 */

void Model::Translate(vec3 tx)
{
    pos = tx;
    trans = translate(tx);
    model = trans * scl * rot;
    modelInv = inverseTranspose(model);
}

/**
 * Scale -
 */

void Model::Scale(vec3 dimsIn)
{
    dims = dimsIn;
    scl = scale(mat4(1.0f), dimsIn);
    model = trans * scl * rot;
    modelInv = inverseTranspose(model);
}

/**
 * Rotate -
 */

void Model::Rotate(vec3 thetas)
{
    angles = thetas;
    mat4 rotx = rotate(thetas[0], vec3(1.0, 0.0, 0.0));
    mat4 roty = rotate(thetas[1], vec3(0.0, 1.0, 0.0));
    mat4 rotz = rotate(thetas[2], vec3(0.0, 0.0, 1.0));

    rot = rotz * roty * rotx;
    model = trans * scl * rot;
    modelInv = inverseTranspose(model);
}