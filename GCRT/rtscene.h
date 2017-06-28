#pragma once

#include "GCRT.h"

using namespace glm;
using namespace std;

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

struct RTCamera
{
    double aperture;
    double fov;
    double shutterSpeed;
    double aspect;
    double tanfov;

    double imageW;
    double imageH;

    mat4 viewInv;
    dvec3 pos;

    void Init(
        uint32_t w,
        uint32_t h,
        dvec3 posIn,
        dvec3 look,
        double fov
    );

    Ray GenerateRay(dvec2 pixel);
};

struct BBox
{
    dvec3 flb;
    dvec3 brt;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTSphere
{
    double r;
    dvec3 orgn;
    RTMaterial mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTMesh
{
    RTMaterial mat;
    vector<dvec3> pos;
    vector<uvec3> tris;
    RTMaterial mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTPlane
{
    dvec4 normal;
    RTMaterial mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTScene
{
    RTCamera cam;
    vector <RTSphere> spheres;
    RTPlane plane;
    dvec3 bgColor;
    void Intersect(Ray ray, Intersection &intsc);
};