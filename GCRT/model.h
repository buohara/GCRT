#pragma once

#include "plane.h"
#include "material.h"
#include "camera.h"
#include "light.h"

struct Model
{
    string meshName;
    string matName;
    vec3 pickerColor;

    vec3 pos;
    vec3 dims;
    vec3 angles;

    mat4 scl;
    mat4 trans;
    mat4 rot;

    mat4 model;
    mat4 modelInv;

    bool selected;

    void SetModelMatrices(GLuint program, shared_ptr<Mesh> pMesh, float t);

    void InitModelMatrices();
    void Translate(vec3 tx);
    void Scale(vec3 dimsIn);
    void Rotate(vec3 thetas);
};