#pragma once

#include "rtcamera.h"
#include "raytrace.h"

struct RTSphere
{
    double r;
    vec3 orgn;
    vec3 ka;
    vec3 kd;

    Intersection Intersect(Ray ray);
};

struct RTPlane
{
    vec4 normal;
    vec3 ka;
    vec3 kd;

    Intersection Intersect(Ray ray);
};

struct RTLight
{
    vec3 pos;
    vec3 color;
};

struct RTScene
{
    RTCamera cam;
    vector <RTLight> lights;
    vector <RTSphere> spheres;
    RTPlane plane;
    vec3 bgColor;
};