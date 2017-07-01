#pragma once

#include "GCRT.h"
#include "rtmaterial.h"
#include "ray.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;

struct RTSphere
{
    double r;
    dvec3 orgn;
    shared_ptr<RTMaterial> mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTPlane
{
    dvec4 normal;
    shared_ptr<RTMaterial> mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTBox
{
    dvec3 min;
    dvec3 max;
    shared_ptr<RTMaterial> mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTMesh
{
    vector<dvec3> pos;
    vector<dvec3> norm;
    vector<uvec3> idcs;
    shared_ptr<RTMaterial> mat;
    void LoadModel(string file);
    void Intersect(Ray ray, Intersection &intsc);
};
