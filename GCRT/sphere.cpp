#include "sphere.h"

void Sphere::Create(vec3 ctr, vec3 rad, uint32_t rings, uint32_t segments)
{
    const float pi = 3.1415926535;

    // Points on the sphere poles.

    verts.push_back(ctr + vec3(0.0, rad.y, 0.0));
    verts.push_back(ctr + vec3(0.0, -rad.y, 0.0));

    // Increments of spherical angles based on number of input rings
    // and segments.

    float dtheta = pi / (rings + 1);
    float dphi = 2.0 * pi /  segments;

    // Generate all the vertices.

    for (uint32_t i = 0; i < segments; i++)
    {
        for (uint32_t j = 1; j <= rings; j++)
        {
            float phi = i * dphi;
            float theta = j * dtheta;

            float x = ctr.x + rad.x * sinf(theta) * cosf(phi);
            float y = ctr.y + rad.y * sinf(theta) * sinf(phi);
            float z = ctr.z + rad.z * cosf(theta);

            verts.push_back(vec3(x, y, z));
        }
    }
}