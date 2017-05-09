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

    map<string, Shader> shaders;
    map<string, Model> models;
    map<string, GLuint> textures;
    map<string, shared_ptr<Material>> materials;
    GLuint dbFboID;

    void HandleInputs(MSG &msg);

    void Init();
    void LoadShaders();
    void LoadTextures();
    void InitMaterials();
    void InitModels();
    void Render(HDC hDC);
};