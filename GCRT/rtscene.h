#pragma once

#include "GCRT.h"

using namespace glm;
using namespace std;

struct Ray
{
    dvec3 org;
    dvec3 dir;
};

struct RTMaterial
{
    string name;

    double krefl;
    double ktrans;
    double kdiff;
    bool emissive;

    dvec3 kd;
    dvec3 ke;

    //dvec3 EvaluateBDSF(Ray ray, RTScene &scn) { return dvec3(0.0, 0.0, 0.0); }
};

struct Intersection
{
    double t;
    dvec3 normal;
    shared_ptr<RTMaterial> mat;
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
    shared_ptr<RTMaterial> mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTMesh
{
    vector<dvec3> pos;
    vector<dvec3> norm;
    vector<uvec3> tris;
    shared_ptr<RTMaterial> mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTPlane
{
    dvec4 normal;
    shared_ptr<RTMaterial> mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTScene
{
    RTCamera cam;
    
    vector<RTSphere> spheres;
    vector<RTMesh> meshes;
    map<string, shared_ptr<RTMaterial>> mats;
    
    RTPlane plane;
    dvec3 bgColor;

    void Intersect(Ray ray, Intersection &intsc);
    void Init();
    void InitMaterials();
    void InitModels();
};