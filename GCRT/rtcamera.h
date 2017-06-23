#pragma once

#include "sampler.h"
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
    vec3 pos;

    void Init(
        uint32_t w,
        uint32_t h,
        vec3 posIn,
        vec3 look,
        double fov
    );
    
    Ray GenerateRay(vec2 pixel);
};