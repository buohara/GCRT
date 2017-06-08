#include "model.h"

/**
 * SetMaterial
 */

void Model::SetMaterial(RMaterial matIn)
{
    mat = matIn;
}

/**
 * SetCamera -
 */

void Model::SetCamera(Camera &cam, GLuint program)
{
    vec3 camPos = cam.pos;
    mat4 proj = cam.GetProjection();
    mat4 view = cam.GetView();

    GLuint viewID = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(viewID, 1, false, &view[0][0]);

    GLuint projID = glGetUniformLocation(program, "proj");
    glUniformMatrix4fv(projID, 1, false, &proj[0][0]);

    GLuint camPosID = glGetUniformLocation(program, "camPos");
    glUniform3fv(camPosID, 1, &camPos[0]);
}

/**
 * SetLights -
 */

void Model::SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights, GLuint program)
{
    mat.SetLights(dirLights, ptLights, program);
}

/**
 * SetMesh -
 */

void Model::SetMesh(GLuint program)
{
    GLuint modelID = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(modelID, 1, false, &model[0][0]);

    GLuint modelInvID = glGetUniformLocation(program, "modelInv");
    glUniformMatrix4fv(modelInvID, 1, false, &modelInv[0][0]);
}

/**
 * ApplyMaterials -
 */

void Model::ApplyMaterial(GLuint program)
{
    mat.ApplyMaterial(program);
}

/**
 * SetUniforms -
 */

void Model::SetUniforms(
    Camera &cam,
    vector<DirectionalLight> &dirLights,
    vector<PointLight> &ptLights,
    GLuint program
)
{
    SetMesh(program);
    SetCamera(cam, program);
    SetLights(dirLights, ptLights, program);
    ApplyMaterial(program);
}

/**
 * Draw -
 */

void Model::Draw()
{
    pMesh->Draw();
}

/**
 * InitVertexObjects -
 */

void Model::InitModelMatrices()
{
    scl = mat4(1.0f);
    rot = mat4(1.0f);
    trans = mat4(1.0f);

    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * Translate -
 */

void Model::Translate(vec3 tx)
{
    pos = tx;
    trans = translate(tx);
    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}

/**
 * Scale -
 */

void Model::Scale(vec3 dimsIn)
{
    dims = dimsIn;
    scl = scale(mat4(1.0f), dimsIn);
    model = trans * rot * scl;
    modelInv = inverseTranspose(model);
}