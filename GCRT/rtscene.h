#pragma once

#include "GCRT.h"
#include "rtmesh.h"
#include "ray.h"
#include "json.hpp"
#include "light.h"
#include "spherelight.h"
#include "glassmaterial.h"
#include "mirrormaterial.h"
#include "mattematerial.h"

using namespace glm;
using namespace std;
using json = nlohmann::json;

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
    
    map<string, shared_ptr<Light>> lights;
    vector<vector<VirtualLight>> vLights;

    map<string, shared_ptr<RTMesh>> meshes;
    map<string, shared_ptr<RTMaterial>> mats;
    map<string, RTModel> models;

    void Intersect(Ray ray, Intersection &intsc);
    void Init();
    void InitMaterials();
    void InitModels();
    
    void GenerateLightPath(
        uint32_t setIdx,
        Ray ray,
        uint32_t maxDepth,
        uint32_t depth,
        dvec3 lightColor
    );
    
    void LoadScene(string filePath);
};