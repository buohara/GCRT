#pragma once

#include "GCRT.h"
#include "ray.h"
#include "texture.h"

using namespace std;
using namespace glm;

struct RTMaterial
{
    string name;
    
    virtual dvec3 EvalBSDF(
        Ray rayIn,
        dvec3 colorIn,
        Intersection intsc,
        Ray rayOut
    ) 
    { 
        return dvec3(0.0, 0.0, 0.0); 
    }
    
    virtual void GetBSDFSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut
    ) 
    {
        return; 
    }

    virtual void Init(uint32_t sampleSets, uint32_t setSize) 
    { 
        return; 
    }
};

struct TexMaterial : RTMaterial
{
    Texture diffTex;
    Texture normTex;
    dvec3 GetDiffuseColor(Ray ray, Intersection intsc);
    void PerturbNormal(Intersection &intsc);
    void Load(string diffTexFile, string normTexFile);
};

struct LightMaterial : RTMaterial
{
    dvec3 lightColor;
    double GetDiffuse(Ray ray, Intersection intsc);
    dvec3 GetEmission(Ray rayIn, Intersection intsc);
};