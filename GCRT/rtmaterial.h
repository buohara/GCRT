#pragma once

#include "GCRT.h"
#include "ray.h"

using namespace std;
using namespace glm;

struct RTMaterial
{
    string name;

    virtual double GetReflectance(Ray ray, Intersection intsc) = 0;
    virtual double GetTransmittance(Ray ray, Intersection intsc) = 0;
    virtual double GetDiffuse(Ray ray, Intersection intsc) = 0;
    virtual dvec3 GetDiffuseColor() { return dvec3(0.0, 0.0, 0.0); }

    virtual void GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut) = 0;
    virtual void GetTransmittedRay(Ray rayIn, Intersection intsc, Ray &rayOut) = 0;
    virtual dvec3 GetEmission(Ray rayIn, Intersection intsc) = 0;
};

struct MatteMaterial : RTMaterial
{
    dvec3 kd;

    double GetReflectance(Ray ray, Intersection intsc);
    double GetTransmittance(Ray ray, Intersection intsc);
    double GetDiffuse(Ray ray, Intersection intsc);
    dvec3 GetDiffuseColor() { return kd; }

    void GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
    void GetTransmittedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
    dvec3 GetEmission(Ray rayIn, Intersection intsc);
};

struct MirrorMaterial : RTMaterial
{
    double GetReflectance(Ray ray, Intersection intsc);
    double GetTransmittance(Ray ray, Intersection intsc);
    double GetDiffuse(Ray ray, Intersection intsc);

    void GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
    void GetTransmittedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
    dvec3 GetEmission(Ray rayIn, Intersection intsc);
};

struct GlassMaterial : RTMaterial
{
    double etai;
    double etat;

    double GetReflectance(Ray ray, Intersection intsc);
    double GetTransmittance(Ray ray, Intersection intsc);
    double GetDiffuse(Ray ray, Intersection intsc);

    void GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
    void GetTransmittedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
    dvec3 GetEmission(Ray rayIn, Intersection intsc);
};

struct LightMaterial : RTMaterial
{
    dvec3 lightColor;

    double GetReflectance(Ray ray, Intersection intsc);
    double GetTransmittance(Ray ray, Intersection intsc);
    double GetDiffuse(Ray ray, Intersection intsc);

    void GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
    void GetTransmittedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
    dvec3 GetEmission(Ray rayIn, Intersection intsc);
};