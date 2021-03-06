#pragma once

#include "gcrt.h"
#include "rtmesh.h"
#include "ray.h"
#include "light.h"
#include "spherelight.h"
#include "timeline.h"
#include "animation.h"
#include "rtcamera.h"

using namespace glm;
using namespace std;

struct VirtualLight
{
    dvec3 pos;
    dvec3 color;
    dvec3 normal;
    string material;
};

struct RTScene
{
    RTCamera cam;
    Timeline tl;
    
    map<string, shared_ptr<Light>> lights;
    vector<vector<VirtualLight>> vLights;

    vector<RTMesh> meshes;
    vector<RTMaterial> mats;

    void Add(RTMaterial mat);
    void Add(RTMesh mesh);
    uint32_t GetMaterialIndex(string matName);

    map<string, uint32_t> matIdcs;

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