#pragma once

#include "GCRT.h"

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
    dvec2 uv;
    string mat;
};