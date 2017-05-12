#pragma once

#include "plane.h"
#include "material.h"
#include "camera.h"
#include "light.h"

struct Model
{
    shared_ptr<Geometry> pGeom;
    RMaterial mat;
    GLuint program;

    void SetMaterial(RMaterial matIn);

    void SetGeometry(GLuint program);
    void ApplyMaterial(GLuint program);
    void SetCamera(Camera &cam, GLuint program);
    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights, GLuint program);

    void SetUniforms(
        Camera &cam,
        vector<DirectionalLight> &dirLights,
        vector<PointLight> &ptLights, 
        GLuint program
    );

    void Draw();
};