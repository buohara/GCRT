#include "rtscene.h"

/**
 * RTScene::Intersect Shoot a ray into the scene and get its
 * intersection.
 *
 * @param  ray Ray to shoot into scene.
 * @param  intsc Output. Intersection information.
 */

void RTScene::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t         = -1.0;
    double minDist  = DBL_MAX;

    Intersection intsc2;

    for (auto &mesh : meshes)
    {
        mesh.Intersect(ray, intsc2);
        if ((intsc2.t < minDist) && (intsc2.t > 0.001))
        {
            intsc = intsc2;
            minDist = intsc2.t;
        }
    }
    
    for (auto &lightKV : lights)
    {
        auto &light = *lightKV.second;
        light.Intersect(ray, intsc2);

        if ((intsc2.t < minDist) && (intsc2.t > 0.001))
        {
            intsc = intsc2;
            minDist = intsc2.t;
            return;
        }
    }
}

/**
 * RTScene::UpdateAnimations - Update scene object animations to specified time.
 *
 * @param t Time to advance to.
 */

void RTScene::UpdateAnimations(double t)
{
    for (auto& mesh : meshes) mesh.UpdateAnimation(t);
}

/**
 * [RTScene::GenerateLightPath description]
 * @param ray        [description]
 * @param maxDepth   [description]
 * @param depth      [description]
 * @param lightColor [description]
 */

void RTScene::GenerateLightPath(
    uint32_t setIdx,
    Ray ray,
    uint32_t maxDepth,
    uint32_t depth,
    dvec3 lightColor
)
{
    return;
}