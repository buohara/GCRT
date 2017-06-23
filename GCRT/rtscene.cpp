#include "rtscene.h"

Intersection RTSphere::Intersect(Ray ray)
{
    Intersection intsc;
    intsc.ka = ka;
    intsc.kd = kd;
    intsc.t = -1.0;

    vec3 d = ray.dir;
    vec3 o = ray.org;
    vec3 cnt = orgn;

    double a = dot(d, d);
    double b = 2 * dot(d, o - cnt);
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
        intsc.normal = o + (float)t1 * d - orgn;
        return intsc;
    }

    if (t2 > 0.0)
    {
        intsc.t = t2;
        intsc.normal = o + (float)t2 * d - orgn;
        return intsc;
    }

    return intsc;
}

Intersection RTPlane::Intersect(Ray ray)
{
    Intersection intsc;
    intsc.ka = ka;
    intsc.kd = kd;
    intsc.t = -1.0;
    intsc.normal = vec3(normal.x, normal.y, normal.z);

    vec3 n = vec3(normal.x, normal.y, normal.z);

    if (dot(n, ray.dir) == 0.0)
    {
        return intsc;
    }

    double d = normal.w;
    intsc.t = (d - dot(n, ray.org)) / (dot(n, ray.dir));
    return intsc;
}