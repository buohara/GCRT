#pragma once

#include "gcrt.h"
#include "ray.h"
#include "texture.h"
#include "coshemisampler.h"

using namespace std;
using namespace glm;

enum MatType
{
    LAMBERT,
    FRESNEL,
    MIRROR,
    BLINN,
    TEX
};

struct RTMaterial
{
    string name;
    MatType type;

    dvec3 EvalBSDF(Ray rayIn, dvec3 colorIn, Intersection intsc, Ray rayOut);
    uint32_t GetBSDFSamples(uint32_t numSamples, Ray rayIn, Intersection intsc, vector<Ray>& raysOut);
    double BSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc);

    RTMaterial() : name(""), kd(dvec3(0.0, 0.0, 0.0)), etai(0.0), etat(0.0), ks(0.0) {}

    // Lambert

    RTMaterial(MatType type, string name, dvec3 kd, uint32_t sampleSetSize, uint32_t numSampleSets);

    // mirror

    RTMaterial(MatType type, string name);

    // Blinn

    RTMaterial(MatType type, string name, double ks);

    // Fresnel

    RTMaterial(MatType type, string name, double etai, double etat);

private:

    // lambert

    dvec3 kd;
    CosHemiSampler sampler;

    double LambertBSDFPDF(Ray sampleRay, Ray camRay, Intersection intsc);
    dvec3 LambertEvalBSDF(Ray sampleRay, dvec3 sampleColor, Intersection intsc, Ray camRay);
    uint32_t LambertGetBSDFSamples(uint32_t numSamples, Ray camRay, Intersection intsc, vector<Ray>& sampleRays);

    // mirror

    dvec3 MirrorEvalBSDF(Ray sampleRay, dvec3 sampleColor, Intersection intsc, Ray camRay);
    uint32_t MirrorGetBSDFSamples(uint32_t numSamples, Ray camRay, Intersection intsc, vector<Ray>& sampleRays);

    // Blinn

    double ks;

    dvec3 BlinnEvalBSDF(Ray sampleRay, dvec3 sampleColor, Intersection intsc, Ray camRay);
    uint32_t BlinnGetBSDFSamples(uint32_t numSamples, Ray camRay, Intersection intsc, vector<Ray>& sampleRays);
    double BlinnBSDFPDF(Ray sampleRay, Ray camRay, Intersection intsc);

    // Fresnel

    double etai;
    double etat;

    double GetReflectance(Ray ray, Intersection intsc);
    void GetTransmittedRay(Ray ray, Intersection intsc, Ray& rayOut);
    void GetReflectedRay(Ray ray, Intersection intsc, Ray& rayOut);

    dvec3 FresnelEvalBSDF(Ray sampleRay, dvec3 sampleColor, Intersection intsc, Ray camRay);
    uint32_t FresnelGetBSDFSamples(uint32_t numSamples, Ray camRay, Intersection intsc, vector<Ray>& sampleRays);
};

struct TexMaterial : RTMaterial
{
    Texture diffTex;
    Texture normTex;
    dvec3 GetDiffuseColor(Ray ray, Intersection intsc);
    void PerturbNormal(Intersection &intsc);
    void Load(string diffTexFile, string normTexFile);
};