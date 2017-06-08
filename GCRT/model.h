#pragma once

#include "plane.h"
#include "material.h"
#include "camera.h"
#include "light.h"

struct Model
{
    shared_ptr<Mesh> pMesh;
    RMaterial mat;
    GLuint program;
    vec3 pickerColor;

    vec3 pos;
    vec3 dims;

    mat4 scl;
    mat4 trans;
    mat4 rot;

    mat4 model;
    mat4 modelInv;

    void SetMaterial(RMaterial matIn);
    void SetMesh(GLuint program);
    void ApplyMaterial(GLuint program);
    void SetCamera(Camera &cam, GLuint program);
    void SetLights(vector<DirectionalLight> &dirLights, vector<PointLight> &ptLights, GLuint program);

    void InitModelMatrices();
    void Translate(vec3 tx);
    void Scale(vec3 dimsIn);

    void SetUniforms(
        Camera &cam,
        vector<DirectionalLight> &dirLights,
        vector<PointLight> &ptLights, 
        GLuint program
    );

    void Draw();
};