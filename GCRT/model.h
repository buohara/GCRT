#pragma once

#include "plane.h"
#include "material.h"

struct Model
{
    unique_ptr<Geometry> pGeom;
    shared_ptr<Material> pMat;

    void SetMaterial(shared_ptr<Material> pMatIn);
    void Draw();
};