#pragma once

#include "GCRT.h"
#include "rtmaterial.h"
#include "ray.h"
#include "Octree.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;

struct RTSphere
{
    double r;
    dvec3 orgn;
    string mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTPlane
{
    dvec4 normal;
    string mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTBox
{
    dvec3 min;
    dvec3 max;
    string mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct Submesh
{
    Octree root;

    vector<dvec3> pos;
    vector<dvec3> norm;
    vector<uvec3> faces;

    string mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTMesh
{
    BBox bbox;
    vector<Submesh> submeshes;

    void CreateCornellBox();
    void LoadModel(string file);
    void Intersect(Ray ray, Intersection &intsc);
};
