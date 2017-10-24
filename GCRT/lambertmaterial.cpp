#include "lambertmaterial.h"

/**
 * [LambertMaterial::EvalBSDF description]
 * @param  rayIn   [description]
 * @param  colorIn [description]
 * @param  intsc   [description]
 * @param  rayOut  [description]
 * @return         [description]
 */

dvec3 LambertMaterial::EvalBSDF(
    Ray rayIn, 
    dvec3 colorIn, 
    Intersection intsc,
    Ray rayOut
)
{
    dvec3 out = 0.3 * colorIn * kd * dot(rayIn.dir, intsc.normal);
    return out;
}

/**
 * [LambertMaterial::GetBSDFSamples description]
 * @param numSamples [description]
 * @param rayIn      [description]
 * @param intsc      [description]
 * @param raysOut    [description]
 * @param weights    [description]
 */

void LambertMaterial::GetBSDFSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut
)
{
    dvec3 org = rayIn.org + intsc.t * rayIn.dir;
    uint32_t sampleSet = sampler.NextSet();

    dvec3 bitan = cross(intsc.normal, intsc.tan);
    dmat3 tbn = dmat3(intsc.tan, bitan, intsc.normal);

    for (uint32_t i = 0; i < numSamples; i++)
    {
        Ray rayOut;
        rayOut.org = org;
        rayOut.dir = normalize(tbn * sampler.samples[sampleSet][i]);
        raysOut.push_back(rayOut);
    }
}

/**
 * [BSDFPDF description]
 * @param  rayIn  [description]
 * @param  rayOut [description]
 * @param  intsc  [description]
 * @return        [description]
 */

double LambertMaterial::BSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc)
{
    dvec3 n = intsc.normal;
    double costheta = dot(rayOut.dir, n);
    return costheta / glm::pi<double>();
}

/**
 * [LambertMaterial::Init description]
 * @param sampleSets [description]
 * @param setSize    [description]
 */

void LambertMaterial::Init(uint32_t sampleSets, uint32_t setSize)
{
    sampler.GenerateSamples(sampleSets, setSize);
}