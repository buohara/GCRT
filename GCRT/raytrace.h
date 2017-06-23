#pragma once

#include "scene.h"

void RTMain();

struct Ray
{
    vec3 org;
    vec3 dir;
};

struct Intersection
{
    double t;
    vec3 normal;
    vec3 ka;
    vec3 kd;
};

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