#include "model.h"

/**
 * SetShaderParams -
 */

void Model::SetShaderParams(GLuint progID)
{
    mat.SetShaderParams(progID);
    geom.SetShaderParams(progID);
};

/**
 * Draw -
 */

void Model::Draw()
{
    geom.Draw();
}