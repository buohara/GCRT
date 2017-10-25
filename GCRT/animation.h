#pragma once

#include "GCRT.h"

using namespace glm;
using namespace std;

struct Keyframe
{
    double t;
    dvec3 pos;
    dvec3 look;
    dvec3 up;
    dvec3 scale;
};

struct Animation
{
    vector<Keyframe> kfs;
    dmat4 GetPose(double t);
};