#pragma once

#include "camera.h"
#include "light.h"
#include "materialgl.h"
#include "meshgl.h"

struct Scene
{
    Camera cam;

    map<string, Tex> diffTextures;
    map<string, Tex> normTextures;
    map<string, RMaterial> materials;
    vector<Light> lights;
    map<string, MeshGL> meshes;

    Tex skyTex;

    Scene(uint32_t w, uint32_t h);

    void AddDiffTexture(string name, string path, GLuint id);
    void AddNormTexture(string name, string path, GLuint id);
    void SetSkyTex(string path, GLuint id);

#if 0
    void Save(string file);
    void Load(string file);
#endif

    void AddMaterial(string name, RMaterial mat);
    void AddMesh(string name, MeshGL &mesh);

private:

    Scene();
};