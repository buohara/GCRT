#pragma once

#include "GCRT.h"
#include "ray.h"

using namespace std;
using namespace glm;

struct BBox
{
    dvec3 min;
    dvec3 max;
    void Intersect(Ray ray, Intersection &intsc);
    bool Contains(dvec3 p0, dvec3 p1, dvec3 p2);
};

struct Octree
{
    BBox box;
    unique_ptr<Octree> children[8];
    Insert()
};