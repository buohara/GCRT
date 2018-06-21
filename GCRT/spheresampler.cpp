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
    curSet = 0;

    for (uint32_t i = 0; i < numSets; i++)
    {
        for (uint32_t j = 0; j < setSize; j++)
        {
            double u = (double)rand() / (double)RAND_MAX;
            double v = (double)rand() / (double)RAND_MAX;

            double theta = acos(1.0 - 2 * v);
            double phi = 2.0 * pi<double>() * u;

            sample.x = sin(theta) * cos(phi);
            sample.y = sin(theta) * sin(phi);
            sample.z = cos(theta);
            sample.w = 1.0;
            samples[i].push_back(sample);
        }
    }
}

/**
 * [SphereSampler::NextSet description]
 * @return [description]
 */

uint32_t SphereSampler::NextSet()
{
    curSet++;
    if (curSet >= samples.size())
    {
        curSet = 0;
    }

    uint32_t outSet = curSet;
    
    return (uint32_t)(outSet >= samples.size() ? samples.size() - 1 : outSet);
}