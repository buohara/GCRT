#pragma once

#include "scene.h"

void RTMain();

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

struct RTMaterial
{
    string name;
    string type;

    double maxAlpha;

    dvec3 kd;
    dvec3 lightColor;
};

struct Ray
{
    dvec3 org;
    dvec3 dir;
};

struct Intersection
{
    double t;
    dvec3 normal;
    RTMaterial mat;
};

struct RTSphere
{
    double r;
    dvec3 orgn;
    RTMaterial mat;
    Intersection Intersect(Ray ray);
};

struct RTPlane
{
    dvec4 normal;
    RTMaterial mat;
    Intersection Intersect(Ray ray);
};

struct RayTracer
{
    static DWORD WINAPI RTThreadFunc(LPVOID lpParam);
};