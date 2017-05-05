#include "model.h"

/**
 * SetMaterial
 */

void Model::SetMaterial(shared_ptr<Material> pMatIn)
{
    pMat = pMatIn;
}

/**
 * Draw -
 */

void Model::Draw()
{
    pMat->ApplyMaterial();
    pGeom->SetShaderParams(pMat->program);
    pGeom->Draw();
}