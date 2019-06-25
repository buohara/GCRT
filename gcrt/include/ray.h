#pragma once

#include "gcrt.h"

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
    bool isLight;
    dvec3 normal;
    dvec2 uv;
    dvec3 tan;
    uint32_t mat;

    Intersection() : t(-1.0), isLight(false), normal(dvec3(0.0)), uv(dvec2(0.0)), tan(dvec3(0.0)), mat(0xFFFFFFFF) {}
};