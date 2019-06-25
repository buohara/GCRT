#pragma once

#include "gcrt.h"
#include "ray.h"

using namespace std;
using namespace glm;

struct BBox
{
    dvec3 min;
    dvec3 max;
    void Intersect(Ray ray, Intersection &intsc);
    bool Contains(dvec3 p0, dvec3 p1, dvec3 p2);
    bool Overlaps(dvec3 p0, dvec3 p1, dvec3 p2);
};

const uint32_t maxOctreeChildren = 8;

struct Octree
{
    BBox box;
    shared_ptr<Octree> children[8];
    vector<uint32_t> faceIdcs;
    uint32_t depth;
    uint32_t maxDepth;

    void Insert(dvec3 p0, dvec3 p1, dvec3 p2, uint32_t face);
    
    void Intersect(Ray ray, const vector<dvec3> &pos, const vector<dvec3>& norm,
        const vector<dvec3>& tan, const vector<uvec3>& faces, Intersection &intsc);
};