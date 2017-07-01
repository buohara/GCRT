#include "Octree.h"

/**
 * [BBox::Intersect description]
 * @param ray   [description]
 * @param intsc [description]
 */

void BBox::Intersect(Ray ray, Intersection &intsc)
{
    double t0 = 0.0;
    double t1 = 0.0;
    double tmin = 0.0;
    double tmax = 0.0;
    double tmp;
    double flip = 1.0;

    // x planes

    double inv = 1.0 / ray.dir.x;
    tmin = (min.x - ray.org.x) * inv;
    tmax = (max.x - ray.org.x) * inv;

    if (tmin > tmax)
    {
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
        flip = -1.0;
    }

    t1 = tmax;
    t0 = tmin;
    intsc.normal = flip * dvec3(1.0, 0.0, 0.0);

    // y planes

    inv = 1.0 / ray.dir.y;
    tmin = (min.y - ray.org.y) * inv;
    tmax = (max.y - ray.org.y) * inv;
    flip = 1.0;

    if (tmin > tmax)
    {
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
        flip = -1.0;
    }

    if (tmax < t0 || tmin > t1)
    {
        intsc.t = -1.0;
        return;
    }

    if (tmin > t0)
    {
        intsc.normal = flip * dvec3(0.0, 1.0, 0.0);
        t0 = tmin;
    }

    if (tmax < t1)
    {
        t1 = tmax;
    }

    // z planes

    inv = 1.0 / ray.dir.z;
    tmin = (min.z - ray.org.z) * inv;
    tmax = (max.z - ray.org.z) * inv;
    flip = 1.0;

    if (tmin > tmax)
    {
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
        flip = -1.0;
    }

    if (tmax < t0 || tmin > t1)
    {
        intsc.t = -1.0;
        return;
    }

    if (tmin > t0)
    {
        t0 = tmin;
        intsc.normal = flip * dvec3(0.0, 0.0, 1.0);
    }

    if (tmax < t1)
    {
        t1 = tmax;
    }

    intsc.t = t0;
}

/**
 * [BBox::Contains description]
 * @param  p0 [description]
 * @param  p1 [description]
 * @param  p2 [description]
 * @return    [description]
 */

bool BBox::Contains(dvec3 p0, dvec3 p1, dvec3 p2)
{
    if (p0.x > max.x || p0.x < min.x ||
        p0.y > max.y || p0.y < min.y ||
        p0.z > max.z || p0.z < min.z
        )
    {
        return false;
    }

    if (p1.x > max.x || p1.x < min.x ||
        p1.y > max.y || p1.y < min.y ||
        p1.z > max.z || p1.z < min.z
        )
    {
        return false;
    }

    if (p2.x > max.x || p2.x < min.x ||
        p2.y > max.y || p2.y < min.y ||
        p2.z > max.z || p2.z < min.z
        )
    {
        return false;
    }

    return true;
}