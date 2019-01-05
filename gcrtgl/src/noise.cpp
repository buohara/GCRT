#include "noise.h"

/**
 * [Noise::CreateGradients description]
 * @param freq [description]
 */

void Noise::CreateGradients(uint32_t freq)
{
    vector<vec2> grads;
    float delta = 1.0f / (float)freq;

    float dx = 0.0;
    
    while (dx < 1.0)
    {
        float dy = 0.0;

        while (dy < 1.0)
        {
            float theta = 2.0f * pi<float>() * (float)rand() / (float)RAND_MAX;
            grads.push_back(vec2(cos(theta), sin(theta)));
            dy += delta;
        }
    
        float theta = 2.0f * pi<float>() * (float)rand() / (float)RAND_MAX;
        grads.push_back(vec2(cos(theta), sin(theta)));
        dx += delta;
    }
	
    float theta = 2.0f * pi<float>() * (float)rand() / (float)RAND_MAX;
    grads.push_back(vec2(cos(theta), sin(theta)));

    gradients[freq] = grads;
}

/**
 * [Noise::GetPerlin description]
 * @param  freq [description]
 * @param  x    [description]
 * @param  y    [description]
 * @return      [description]
 */

float Noise::GetPerlin(uint32_t freq, float x, float y)
{
    if (gradients.count(freq) == 0)
    {
        CreateGradients(freq);
    }

    vector<vec2> &grads = gradients[freq];
    float delta = 1.0f / (float)freq;
    
    uint32_t xi = (uint32_t)(x / delta);
    uint32_t yi = (uint32_t)(y / delta);

    vec2 g00 = grads[yi * (freq + 1) + xi];
    vec2 g01 = grads[yi * (freq + 1) + (xi + 1)];
    vec2 g10 = grads[(yi + 1) * (freq + 1) + xi];
    vec2 g11 = grads[(yi + 1) * (freq + 1) + (xi + 1)];

	return 0.0f;
}