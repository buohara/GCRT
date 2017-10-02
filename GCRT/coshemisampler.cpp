#include "coshemisampler.h"

/**
 * [CosHemiSampler::GenerateSamples description]
 * @param numSets [description]
 * @param setSize [description]
 */

void CosHemiSampler::GenerateSamples(uint32_t numSets, uint32_t setSize)
{
    samples.resize(numSets);
    curSet = 0;

    dvec4 sample;

    for (uint32_t i = 0; i < numSets; i++)
    {
        for (uint32_t j = 0; j < setSize; j++)
        {
            double r = sqrt((double)rand() / (double)RAND_MAX);
            double phi = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;

            sample.x = r * cos(phi);
            sample.y = r * sin(phi);
            sample.z = max(0.0, sqrt(1.0 - sample.x * sample.x - sample.y * sample.y));
            sample.w = 1.0;
            samples[i].push_back(sample);
        }
    }
}

/**
 * [CosHemiSampler::NextSet description]
 * @return [description]
 */

uint32_t CosHemiSampler::NextSet()
{
    curSet++;
    if (curSet >= samples.size())
    {
        curSet = 0;
    }

    uint32_t outSet = curSet;

    return (outSet >= 16 ? 15 : outSet);
}