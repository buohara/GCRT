#include "sampler.h"

/**
 * [Sampler::GenerateSamples description]
 * @param numSamples [description]
 * @param x          [description]
 * @param y          [description]
 * @param samples    [description]
 */

void Sampler::GenerateSamples(
    uint32_t x,
    uint32_t y,
    vector<dvec2> &samples
)
{
    samples.resize(numSamples);

    double px = (double)x;
    double py = (double)y;

    dvec2 sample;

    for (uint32_t i = 0; i < numSamples; i++)
    {
        double r1 = (double)rand() / (double)RAND_MAX;
        double r2 = (double)rand() / (double)RAND_MAX;

        sample.x = px + r1;
        sample.y = py + r2;
        samples[i] = sample;
    }
}