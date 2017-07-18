#pragma once

#include "GCRT.h"
#include "rtmesh.h"
#include "ray.h"

using namespace glm;
using namespace std;

struct RTCamera
{
    double aperture;
    double focalDist;
    double fov;
    double shutterSpeed;
    double aspect;
    double tanfov;

    double imageW;
    double imageH;

    uint32_t numDOFRays;

    mat4 viewInv;
    
    dvec3 pos;
    dvec3 lookDir;

    void Init(
        uint32_t w,
        uint32_t h,
        dvec3 posIn,
        dvec3 look,
        double fov
    );

    Ray GeneratePrimaryRay(dvec2 pixel);
    Ray GenerateSecondaryRay(Ray primRay, dvec2 pixel);
};

struct RTScene
{
    RTCamera cam;
    
    vector<RTSphere> spheres;
    vector<RTBox> boxes;
    vector<shared_ptr<RTMesh>> meshes;
    map<string, shared_ptr<RTMaterial>> mats;
    
    RTPlane plane;
    dvec3 bgColor;

    void Intersect(Ray ray, Intersection &intsc);
    void Init();
    void InitMaterials();
    void InitModels();
};