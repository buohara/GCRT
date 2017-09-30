#include "mattematerial.h"

/**
 * [MatteMaterial::EvalBSDF description]
 * @param  rayIn   [description]
 * @param  colorIn [description]
 * @param  intsc   [description]
 * @param  rayOut  [description]
 * @return         [description]
 */

dvec3 MatteMaterial::EvalBSDF(
    Ray rayIn, 
    dvec3 colorIn, 
    Intersection intsc,
    Ray rayOut
)
{
    return colorIn * dot(-rayIn.dir, intsc.normal);
}

/**
 * [MatteMaterial::GetBSDFSamples description]
 * @param numSamples [description]
 * @param rayIn      [description]
 * @param intsc      [description]
 * @param raysOut    [description]
 * @param weights    [description]
 */

void MatteMaterial::GetBSDFSamples(
    uint32_t numSamples,
    Ray rayIn,
    Intersection intsc,
    vector<Ray> &raysOut
)
{
    dvec3 org = rayIn.org + intsc.t * rayIn.dir;
    uint32_t sampleSet = sampler.NextSet();

    for (uint32_t i = 0; i < numSamples; i++)
    {
        Ray rayOut;
        rayOut.org = org;
        rayOut.dir = sampler.samples[sampleSet][i];
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

double MatteMaterial::BSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc)
{
    dvec3 n = intsc.normal;
    double costheta = dot(rayOut.dir, n);
    return costheta / glm::pi<double>();
}

/**
 * [MatteMaterial::Init description]
 * @param sampleSets [description]
 * @param setSize    [description]
 */

void MatteMaterial::Init(uint32_t sampleSets, uint32_t setSize)
{
    sampler.GenerateSamples(sampleSets, setSize);
}