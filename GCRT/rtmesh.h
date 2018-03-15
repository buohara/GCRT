#pragma once

#include "GCRT.h"
#include "rtmaterial.h"
#include "ray.h"
#include "Octree.h"
#include "animation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;

struct Submesh
{
    Octree root;
    vector<dvec3> pos;
    vector<dvec3> norm;
    vector<dvec3> tan;
    vector<uvec3> faces;

    string mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTMesh
{
    BBox bbox;
    vector<Submesh> submeshes;
    virtual void Intersect(Ray ray, Intersection &intsc) = 0;
    virtual void UpdateAnimation(double t) { return; }
};

struct SkeletalMesh : RTMesh
{
    void LoadModel(string file);
    void Intersect(Ray ray, Intersection &intsc);
};

struct JSONMesh : RTMesh
{
    void LoadModel(string file);
    void Intersect(Ray ray, Intersection &intsc);
};

struct CornellBox : RTMesh
{
    void Create();
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTSphere : RTMesh
{
    double r;
    dvec3 orgn;
    string mat;
    Animation animation;
    void Intersect(Ray ray, Intersection &intsc);
    void UpdateAnimation(double t);
};

struct RTPlane : RTMesh
{
    dvec4 normal;
    string mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTBox : RTMesh
{
    dvec3 min;
    dvec3 max;
    string mat;
    void Intersect(Ray ray, Intersection &intsc);
};

struct RTModel
{
    string mesh;
    string material;
};