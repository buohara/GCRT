#pragma once

#include "gcrt.h"
#include "rtmaterial.h"
#include "ray.h"
#include "octree.h"
#include "animation.h"
#include "ply.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;

enum MeshType
{
    ASSIMP,
    CORNELL,
    SPHERE,
    PLANE,
    BOX,
    PLY
};

struct Submesh
{
    Octree root;
    vector<dvec3> pos;
    vector<dvec3> norm;
    vector<dvec3> tan;
    vector<uvec3> faces;
    uint32_t mat;

    void Intersect(Ray ray, Intersection &intsc);
};

struct RTMesh
{
    BBox bbox;
    vector<Submesh> submeshes;
    MeshType type;
    uint32_t mat;

    // Sphere parameters.

    double r;
    dvec3 orgn;

    // Plane parameters.

    dvec4 normal;

    // Box parameters.

    dvec3 min;
    dvec3 max;

    // Mesh loading routines.

    void LoadModel(string file);
    void LoadAssimpModel(string file);
    void LoadPLYModel(string &fileName);
    
    void Intersect(Ray ray, Intersection &intsc);
    void UpdateAnimation(double t) {};

    RTMesh(MeshType type);
    RTMesh(MeshType type, MaterialType mat, string fileName);
    RTMesh(MeshType type, MaterialType mat, double r, dvec3 orgn);
    RTMesh(MeshType type, MaterialType mat, dvec3 min, dvec3 max);
    RTMesh(MeshType type, MaterialType mat, dvec4 normal);

private:

    void SphereIntersect(Ray ray, Intersection &intsc);
    void PlaneIntersect(Ray ray, Intersection &intsc);
    void BoxIntersect(Ray ray, Intersection &intsc);
    void SkeletalMeshIntersect(Ray ray, Intersection &intsc);
    void CornellBoxIntersect(Ray ray, Intersection &intsc);
    void CreateCornell();
};

struct RTModel
{
    string mesh;
    uint32_t material;
};