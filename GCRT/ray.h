#pragma once

#include "GCRT.h"
#include "rtmaterial.h"

using namespace std;
using namespace glm;

struct Ray
{
    dvec3 org;
    dvec3 dir;
};

struct Intersection
{
    double t;
    dvec3 normal;
    shared_ptr<RTMaterial> mat;
};