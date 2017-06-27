#include "rtscene.h"

/**
 * [RTSphere::Intersect description]
 * @param  ray [description]
 * @return     [description]
 */

Intersection RTSphere::Intersect(Ray ray)
{
    Intersection intsc;
    intsc.t = -1.0;
    intsc.mat = mat;

    dvec3 d = ray.dir;
    dvec3 o = ray.org;
    dvec3 cnt = orgn;

    double a = dot(d, d);
    double b = 2.0 * dot(d, o - cnt);
    double c = dot(o - cnt, o - cnt) - r * r;

    double det = b * b - 4.0 * a * c;

    if (det < 0.0)
    {
        return intsc;
    }

    double t1 = (-b + sqrt(det)) / (2.0 * a);
    double t2 = (-b - sqrt(det)) / (2.0 * a);

    if (t1 <= t2 && t1 > 0.0)
    {
        intsc.t = t1;
        intsc.normal = normalize(o + t1 * d - orgn);
        return intsc;
    }

    if (t2 > 0.0)
    {
        intsc.t = t2;
        intsc.normal = normalize(o + t2 * d - orgn);
        return intsc;
    }

    return intsc;
}

/**
 * [RTPlane::Intersect description]
 * @param  ray [description]
 * @return     [description]
 */

Intersection RTPlane::Intersect(Ray ray)
{
    Intersection intsc;
    intsc.t = -1.0;
    intsc.normal = dvec3(normal.x, normal.y, normal.z);
    intsc.mat = mat;

    dvec3 n = dvec3(normal.x, normal.y, normal.z);

    if (dot(n, ray.dir) == 0.0)
    {
        return intsc;
    }

    double d = normal.w;
    intsc.t = (d - dot(n, ray.org)) / (dot(n, ray.dir));
    return intsc;
}

/**
 * [RTScene::Intersect description]
 * @param  ray [description]
 * @return     [description]
 */

Intersection RTScene::Intersect(Ray ray)
{
    Intersection intsc1;
    intsc1.t = -1.0;
    double minDist = DBL_MAX;

    Intersection intsc2 = plane.Intersect(ray);

    if ((intsc2.t < minDist) && (intsc2.t > 0.0))
    {
        intsc1 = intsc2;
        minDist = intsc2.t;
    }

    for (uint32_t i = 0; i < spheres.size(); i++)
    {
        intsc2 = spheres[i].Intersect(ray);

        if ((intsc2.t < minDist) && (intsc2.t > 0.0))
        {
            intsc1 = intsc2;
            minDist = intsc2.t;
        }
    }

    return intsc1;
}