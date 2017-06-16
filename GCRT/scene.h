#pragma once

#include "box.h"
#include "camera.h"
#include "plane.h"
#include "cylinder.h"
#include "skeletalmesh.h"
#include "sphere.h"
#include "model.h"
#include "shader.h"
#include "light.h"

struct Scene
{
    Camera cam;

    map<string, Model> models;
    map<string, Tex> diffTextures;
    map<string, Tex> normTextures;
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

    void AddDiffTexture(string name, string path, GLuint id);
    void AddNormTexture(string name, string path, GLuint id);

    void Save(string file);
    void Load(string file);

    void AddModel(string name, Model model);
    void AddMaterial(string name, RMaterial mat);
    void AddMesh(string name, shared_ptr<Mesh> pGeom);
    
    void LoadModelFromFile(
        string name,
        string modelFile,
        string diffuseTexFile,
        string normalTexFile,
        vec3 pickerColor,
        bool meshOnly
    );
};