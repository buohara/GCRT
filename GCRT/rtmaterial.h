#pragma once

#include "GCRT.h"
#include "ray.h"
#include "texture.h"

using namespace std;
using namespace glm;

struct RTMaterial
{
    string name;

    virtual double GetReflectance(Ray ray, Intersection intsc) { return 0.0; };
    virtual double GetTransmittance(Ray ray, Intersection intsc) { return 0.0; };
    virtual double GetDiffuse(Ray ray, Intersection intsc) { return 1.0; };
    virtual dvec3 GetDiffuseColor(Ray ray, Intersection intsc) { return dvec3(0.3, 0.3, 0.3); }
    virtual void PerturbNormal(Intersection &intsc) { return; }

    virtual void GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut) { return; }
    virtual void GetTransmittedRay(Ray rayIn, Intersection intsc, Ray &rayOut) { return; }
    virtual dvec3 GetEmission(Ray rayIn, Intersection intsc) { return dvec3(0.0, 0.0, 0.0); }
};

struct TexMaterial : RTMaterial
{
    Texture diffTex;
    Texture normTex;
    dvec3 GetDiffuseColor(Ray ray, Intersection intsc);
    void PerturbNormal(Intersection &intsc);
    void Load(string diffTexFile, string normTexFile);
};

struct MatteMaterial : RTMaterial
{
    dvec3 kd;
    dvec3 GetDiffuseColor(Ray ray, Intersection intsc) { return kd; }
};

struct MirrorMaterial : RTMaterial
{
    double GetReflectance(Ray ray, Intersection intsc) { return 1.0; }
    double GetDiffuse(Ray ray, Intersection intsc) { return 0.0; }
    void GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
};

struct GlassMaterial : RTMaterial
{
    double etai;
    double etat;

    double GetReflectance(Ray ray, Intersection intsc);
    double GetTransmittance(Ray ray, Intersection intsc);
    double GetDiffuse(Ray ray, Intersection intsc) { return 0.0; }

    void GetReflectedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
    void GetTransmittedRay(Ray rayIn, Intersection intsc, Ray &rayOut);
};

struct LightMaterial : RTMaterial
{
    dvec3 lightColor;
    double GetDiffuse(Ray ray, Intersection intsc);
    dvec3 GetEmission(Ray rayIn, Intersection intsc);
};