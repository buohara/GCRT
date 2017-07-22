#pragma once

#include "GCRT.h"
#include "rtmesh.h"
#include "ray.h"

using namespace glm;
using namespace std;

struct VirtualLight
{
    dvec3 pos;
    dvec3 color;
    dvec3 normal;
};

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

    uint32_t dofSamples;

    mat4 viewInv;
    
    dvec3 pos;
    dvec3 lookDir;

    void Init(
        uint32_t w,
        uint32_t h,
        dvec3 posIn,
        dvec3 look,
        double fov,
        double apertureIn,
        double focalDistIn,
        uint32_t dofSamplesIn
    );

    Ray GeneratePrimaryRay(dvec2 pixel);
    Ray GenerateSecondaryRay(Ray primRay, dvec2 pixel);
};

struct RTScene
{
    RTCamera cam;
    
    vector<RTSphere> spheres;
    vector<RTSphere> lights;
    vector<VirtualLight> vLights;

    vector<shared_ptr<RTMesh>> meshes;
    map<string, shared_ptr<RTMaterial>> mats;

    void Intersect(Ray ray, Intersection &intsc);
    void Init();
    void InitMaterials();
    void InitModels();
};