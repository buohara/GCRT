#pragma once

#include "rtcamera.h"
#include "raytrace.h"

struct RTScene
{
    RTCamera cam;
    vector <RTSphere> spheres;
    RTPlane plane;
    dvec3 bgColor;
    Intersection Intersect(Ray ray);
};