#include "spheresampler.h"

/**
 * [SphereSampler::GenerateSamples description]
 * @param numSets [description]
 * @param setSize [description]
 */

void SphereSampler::GenerateSamples(uint32_t numSets, uint32_t setSize)
{
    samples.resize(numSets);
    dvec4 sample;

    for (uint32_t i = 0; i < numSets; i++)
    {
        for (uint32_t j = 1; j < setSize; j++)
        {
            double theta = pi<double>() * (double)rand() / (double)RAND_MAX;
            double phi = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;

            sample.x = sin(theta) * cos(phi);
            sample.y = sin(theta) * sin(phi);
            sample.z = cos(theta);
            sample.w = 1.0;
            samples[i].push_back(sample);
        }
    }
}