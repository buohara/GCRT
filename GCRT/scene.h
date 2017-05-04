#pragma once

#include "GCRT.h"
#include "box.h"
#include "camera.h"
#include "plane.h"
#include "cylinder.h"
#include "sphere.h"
#include "model.h"
#include "shader.h"

struct Scene
{
    Camera cam;
    bool wireFrame = false;

    vector<Shader> shaders;
    vector<Model> models;
    vector<Material> materials;

    void HandleInputs(MSG &msg);
};