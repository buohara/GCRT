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

    double r1 = (double)rand() / (double)RAND_MAX;
    double r2 = (double)rand() / (double)RAND_MAX;

    sample.x = px + r1;
    sample.y = py + r2;
    samples[0] = sample;

    r1 = (double)rand() / (double)RAND_MAX;
    r2 = (double)rand() / (double)RAND_MAX;

    sample.x = px + r1;
    sample.y = py + r2;
    samples[1] = sample;

    r1 = (double)rand() / (double)RAND_MAX;
    r2 = (double)rand() / (double)RAND_MAX;

    sample.x = px + r1;
    sample.y = py + r2;
    samples[2] = sample;

    r1 = (double)rand() / (double)RAND_MAX;
    r2 = (double)rand() / (double)RAND_MAX;

    sample.x = px + r1;
    sample.y = py + r2;
    samples[3] = sample;
}