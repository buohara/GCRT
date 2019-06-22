#pragma once

#include "gcrt.h"
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
    
    virtual uint32_t GetBSDFSamples(
        uint32_t numSamples,
        Ray rayIn,
        Intersection intsc,
        vector<Ray> &raysOut
    ) 
    {
        return 0; 
    }

    virtual double BSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc)
    {
        return 0.0;
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