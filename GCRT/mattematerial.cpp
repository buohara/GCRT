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
    vector<Ray> &raysOut,
    vector<double> &weights
)
{
    dvec3 org = rayIn.org + intsc.t * rayIn.dir;
    uint32_t sampleSet = sampler.NextSet();
    double piInv = 1.0 / glm::pi<double>();

    for (uint32_t i = 0; i < numSamples; i++)
    {
        Ray rayOut;
        rayOut.org = org;
        rayOut.dir = sampler.samples[sampleSet][i];

        double theta = asin(rayOut.dir.z);
        double p = cos(theta) * piInv;

        raysOut.push_back(rayOut);
        weights.push_back(p);
    }
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