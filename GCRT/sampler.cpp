#include "sampler.h"

void Sampler::GenerateSamples(
    uint32_t numSamples,
    uint32_t x,
    uint32_t y,
    vector<vec2> &samples
)
{
    double px = (double)x;
    double py = (double)y;

    vec2 sample;
    sample.x = px + 0.5;
    sample.y = py + 0.5;

    samples[0] = sample;
}