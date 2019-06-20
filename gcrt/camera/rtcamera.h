#pragma once

#include "gcrt.h"
#include "ray.h"

enum CamType
{
    ORTHO,
    PERSPECTIVE
};

struct RTCamera
{
    CamType type;

    double aperture;
    double focalDist;
    double fov;
    double shutterSpeed;
    double aspect;
    double tanfov;

    uint32_t imageW;
    uint32_t imageH;

    mat4 viewInv;

    dvec3 pos;
    dvec3 lookDir;

    RTCamera(uint32_t w, uint32_t h, dvec3 posIn, dvec3 look, double fov,
        double apertureIn, double focalDistIn, CamType type);

    void Init(
        uint32_t w,
        uint32_t h,
        dvec3 posIn,
        dvec3 look,
        double fov,
        double apertureIn,
        double focalDistIn,
        CamType type
    );

    Ray GeneratePrimaryRay(dvec2 pixel);
    Ray GenerateSecondaryRay(Ray primRay, dvec2 pixel);

private:

    Ray GeneratePrimaryRayPersp(dvec2 pixel);
    Ray GeneratePrimaryRayOrtho(dvec2 pixel);
};