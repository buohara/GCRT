#include "lambertmaterial.h"

/**
 * LambertMaterial::EvalBSDF Evaluate surface BSDF for given input and
 * output rays.
 *
 * @param  rayOut  Outgoing ray (going away from camera).
 * @param  colorIn Color returned along outgoing ray.
 * @param  intsc   Surface intersection data.
 * @param  rayIn   Incoming ray (coming from camera).
 * @return         BSDF value.
 */

dvec3 LambertMaterial::EvalBSDF(
    Ray rayOut, 
    dvec3 colorIn, 
    Intersection intsc,
    Ray rayIn
)
{
    dvec3 out = 0.3 * colorIn * kd * dot(rayOut.dir, intsc.normal);
    return out;
}

/**
 * LambertMaterial::GetBSDFSamples Generate rays samples for Monte Carlo estimator.
 *
 * @param numSamples Number of samples requested.
 * @param rayIn      Ray coming from camera.
 * @param intsc      Surface intersection data.
 * @param raysOut    Out sample rays (going away from camera).
 *
 * @return           Number of samples generated (some materials, like perfect mirrors,
 *                   only generate one output ray).
 */

uint32_t LambertMaterial::GetBSDFSamples(
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
        raysOut[i].org = org;
        raysOut[i].dir = normalize(tbn * sampler.samples[sampleSet][i]);
    }

    return numSamples;
}

/**
 * LambertMaterial::BSDFPDF Return PDF for given sample ray. Used for weighting
 * samples in Monte Carlo estimator.
 *
 * @param  rayIn  Ray coming from camera.
 * @param  rayOut Sample ray to get PDF for.
 * @param  intsc  Surface intersection data.
 * @return        PDF for sample ray.
 */

double LambertMaterial::BSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc)
{
    dvec3 n = intsc.normal;
    double costheta = dot(rayOut.dir, n);
    return costheta / glm::pi<double>();
}

/**
 * LambertMaterial::Init Initialize the material. Statically generate sample
 * rays from material distribution for subsequent sample ray requests.
 *
 * @param sampleSets Number of sample sets to generate.
 * @param setSize    Number of rays in each sample set.
 */

void LambertMaterial::Init(uint32_t sampleSets, uint32_t setSize)
{
    sampler.GenerateSamples(sampleSets, setSize);
}