#include "rtmaterial.h"

/**
 * RTMaterial::RTMaterial - Initialize a Lambert material.
 */

RTMaterial::RTMaterial(MatType type, string name, dvec3 kd, uint32_t sampleSetSize, uint32_t numSampleSets) :
    type(type), name(name), kd(kd)
{
    assert(type == LAMBERT);
    sampler.GenerateSamples(numSampleSets, sampleSetSize);
}

/**
 * RTMaterial::LambertEvalBSDF Evaluate surface BSDF for given input and
 * output rays.
 *
 * @param  rayOut  Outgoing ray (going away from camera).
 * @param  colorIn Color returned along outgoing ray.
 * @param  intsc   Surface intersection data.
 * @param  rayIn   Incoming ray (coming from camera).
 * @return         BSDF value.
 */

dvec3 RTMaterial::LambertEvalBSDF(
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
 * RTMaterial::LambertGetBSDFSamples Generate rays samples for Monte Carlo estimator.
 *
 * @param numSamples Number of samples requested.
 * @param rayIn      Ray coming from camera.
 * @param intsc      Surface intersection data.
 * @param raysOut    Out sample rays (going away from camera).
 *
 * @return           Number of samples generated (some materials, like perfect mirrors,
 *                   only generate one output ray).
 */

uint32_t RTMaterial::LambertGetBSDFSamples(
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
 * RTMaterial::LambertBSDFPDF Return PDF for given sample ray. Used for weighting
 * samples in Monte Carlo estimator.
 *
 * @param  rayIn  Ray coming from camera.
 * @param  rayOut Sample ray to get PDF for.
 * @param  intsc  Surface intersection data.
 * @return        PDF for sample ray.
 */

double RTMaterial::LambertBSDFPDF(Ray rayIn, Ray rayOut, Intersection intsc)
{
    dvec3 n = intsc.normal;
    double costheta = dot(rayOut.dir, n);
    return costheta / glm::pi<double>();
}