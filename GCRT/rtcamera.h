#pragma once

#include "GCRT.h"
#include "raytrace.h"

using namespace glm;
using namespace std;

struct RTCamera
{
    double aperture;
    double fov;
    double shutterSpeed;
    double aspect;
    double tanfov;

    double imageW;
    double imageH;

    mat4 viewInv;
    dvec3 pos;

    void Init(
        uint32_t w,
        uint32_t h,
        dvec3 posIn,
        dvec3 look,
        double fov
    );
    
    Ray GenerateRay(dvec2 pixel);
};