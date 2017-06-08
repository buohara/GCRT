#pragma once

#include "box.h"
#include "camera.h"
#include "plane.h"
#include "cylinder.h"
#include "blendermesh.h"
#include "sphere.h"
#include "model.h"
#include "shader.h"
#include "light.h"

struct Scene
{
    Camera cam;

    map<string, Shader> shaders;
    map<string, Model> models;
    map<string, GLuint> diffTextures;
    map<string, GLuint> normTextures;
    map<string, RMaterial> materials;
    vector<DirectionalLight> dirLights;
    vector<PointLight> ptLights;
    map<string, shared_ptr<Mesh>> meshes;

    vector<const char*> diffTexNames;
    vector<const char*> normTexNames;
    vector<const char*> modelNames;
    vector<const char*> materialNames;
    vector<const char*> meshNames;

    Scene();

    void AddDiffTexture(string name, GLuint id);
    void AddNormTexture(string name, GLuint id);

    void Save(string file);
    void Load(string file);

    void AddModel(string name, Model model);
    void AddMaterial(string name, RMaterial mat);
    void AddMesh(string name, shared_ptr<Mesh> pGeom);
    
    void LoadBlenderModel(
        string name,
        string blendFile,
        string diffuseTexFile,
        string normalTexFile,
        vec3 pickerColor
    );
};