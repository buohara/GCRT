#pragma once

#include "rtmaterial.h"

struct GlassMaterial : RTMaterial
{
    double etai;
    double etat;

    double GetReflectance(Ray ray, Intersection intsc);
    void GetTransmittedRay(Ray ray, Intersection intsc, Ray &rayOut);
    void GetReflectedRay(Ray ray, Intersection intsc, Ray &rayOut);

    dvec3 EvalBDRF(Ray rayIn, dvec3 colorIn, Ray rayOut);
    
    void GetSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut,
        vector<double> &weights
    );
};