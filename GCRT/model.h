#pragma once

#include "plane.h"
#include "material.h"
#include "camera.h"
#include "light.h"

struct Model
{
    shared_ptr<Geometry> pGeom;
    shared_ptr<Material> pMat;
    GLuint program;

    void SetMaterial(shared_ptr<Material> pMatIn);
    void SetCamera(Camera &cam);
    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights);

    void Draw();
};