#include "sampler.h"

/**
 * [Sampler::GenerateSamples description]
 * @param numSamples [description]
 * @param x          [description]
 * @param y          [description]
 * @param samples    [description]
 */

void Sampler::GenerateSamples(
    uint32_t numSamples,
    uint32_t x,
    uint32_t y,
    vector<dvec2> &samples
)
{
    double px = (double)x;
    double py = (double)y;

    dvec2 sample;
    sample.x = px + 0.5;
    sample.y = py + 0.5;

    samples[0] = sample;
}