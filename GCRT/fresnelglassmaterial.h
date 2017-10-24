#pragma once

#include "rtmaterial.h"

struct FresnelGlassMaterial : RTMaterial
{
    double etai;
    double etat;

    double GetReflectance(Ray ray, Intersection intsc);
    void GetTransmittedRay(Ray ray, Intersection intsc, Ray &rayOut);
    void GetReflectedRay(Ray ray, Intersection intsc, Ray &rayOut);

    dvec3 EvalBSDF(
        Ray rayIn,
        dvec3 colorIn,
        Intersection intsc,
        Ray rayOut
    );
    
    void GetBSDFSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut
    );
};