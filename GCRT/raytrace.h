#pragma once

#include "scene.h"

struct RayTraceIn
{
    map<string, Model> models;
    map<string, Tex> diffTextures;
    map<string, Tex> normTextures;
    map<string, RMaterial> materials;
    vector<DirectionalLight> dirLights;
    vector<PointLight> ptLights;
    map<string, shared_ptr<Mesh>> meshes;
};

struct RayTracer
{
    static DWORD WINAPI RTThreadFunc(LPVOID lpParam);
};