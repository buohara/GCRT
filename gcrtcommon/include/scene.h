#pragma once

#include "camera.h"
#include "light.h"
#include "material.h"
#include "meshgl.h"

struct Scene
{
    Camera cam;

    map<string, Tex> diffTextures;
    map<string, Tex> normTextures;
    map<string, RMaterial> materials;
    vector<DirectionalLight> dirLights;
    vector<PointLight> ptLights;
    map<string, MeshGL> meshes;

    vector<const char*> diffTexNames;
    vector<const char*> normTexNames;
    vector<const char*> modelNames;
    vector<const char*> materialNames;
    vector<const char*> meshNames;

    Tex skyTex;

    Scene();

    void AddDiffTexture(string name, string path, GLuint id);
    void AddNormTexture(string name, string path, GLuint id);
    void SetSkyTex(string path, GLuint id);

#if 0
    void Save(string file);
    void Load(string file);
#endif

    void AddMaterial(string name, RMaterial mat);
    void AddMesh(string name, MeshGL &mesh);
};