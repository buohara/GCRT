#pragma once

#include "plane.h"
#include "material.h"
#include "camera.h"

struct Model
{
    shared_ptr<Geometry> pGeom;
    shared_ptr<Material> pMat;
    GLuint program;

    void SetMaterial(shared_ptr<Material> pMatIn);
    void SetCamera(Camera &cam);
    void SetLights(vec3 light);

    void Draw();
};