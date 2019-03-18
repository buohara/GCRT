#pragma once

#include "gcrt.h"
#include "rtmesh.h"
#include "ray.h"
#include "light.h"
#include "spherelight.h"
#include "fresnelglassmaterial.h"
#include "mirrormaterial.h"
#include "lambertmaterial.h"
#include "blinnmaterial.h"
#include "timeline.h"
#include "animation.h"

using namespace glm;
using namespace std;

struct VirtualLight
{
    dvec3 pos;
    dvec3 color;
    dvec3 normal;
    string material;
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
        double focalDistIn
    );

    Ray GeneratePrimaryRay(dvec2 pixel);
    Ray GenerateSecondaryRay(Ray primRay, dvec2 pixel);
};

struct RTScene
{
    RTCamera cam;
    Timeline tl;
    
    map<string, shared_ptr<Light>> lights;
    vector<vector<VirtualLight>> vLights;

    map<string, shared_ptr<RTMesh>> meshes;
    map<uint32_t, shared_ptr<RTMaterial>> mats;
    map<string, RTModel> models;

    void LoadDefaultScene(uint32_t w, uint32_t h);
    void InitDefaultMaterials();
    void InitDefaultModels();

    void Intersect(Ray ray, Intersection &intsc);
    void UpdateAnimations(double t);

    void GenerateLightPath(
        uint32_t setIdx,
        Ray ray,
        uint32_t maxDepth,
        uint32_t depth,
        dvec3 lightColor
    );
};