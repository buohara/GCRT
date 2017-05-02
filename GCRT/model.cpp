#include "model.h"

/**
 * SetShaderParams -
 */

void Model::SetShaderParams(GLuint progID)
{
    mat.SetShaderParams(progID);
    pGeom->SetShaderParams(progID);
};

/**
 * SetMaterial
 */

void Model::SetMaterial(const Material &matIn)
{
    mat.ka = matIn.ka;
    mat.kd = matIn.kd;
    mat.name = matIn.name;
    mat.diffuseTexID = matIn.diffuseTexID;
    mat.normalTexID = matIn.normalTexID;
}

/**
 * Draw -
 */

void Model::Draw()
{
    pGeom->Draw();
}