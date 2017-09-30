#pragma once

#include "rtmaterial.h"

struct GlassMaterial : RTMaterial
{
    double etai;
    double etat;

    double GetReflectance(Ray ray, Intersection intsc);
    void GetTransmittedRay(Ray ray, Intersection intsc, Ray &rayOut);
    void GetReflectedRay(Ray ray, Intersection intsc, Ray &rayOut);

    double BSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc);

    dvec3 EvalBSDF(
        Ray rayIn,
        Intersection intsc,
        dvec3 colorIn,
        Ray rayOut
    );
    
    void GetBSDFSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut
    );
};