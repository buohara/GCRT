#pragma once

#include "box.h"
#include "camera.h"
#include "plane.h"
#include "cylinder.h"
#include "sphere.h"
#include "model.h"
#include "shader.h"
#include "light.h"

struct Scene
{
    Camera cam;

    map<string, Shader> shaders;
    map<string, Model> models;
    map<string, GLuint> textures;
    map<string, RMaterial> materials;

    vector<DirectionalLight> dirLights;
    vector<PointLight> ptLights;
};