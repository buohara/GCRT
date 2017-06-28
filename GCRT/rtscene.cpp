#include "rtscene.h"

/**
 * [RTSphere::Intersect description]
 * @param  ray [description]
 */

void RTSphere::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t = -1.0;

    dvec3 d = ray.dir;
    dvec3 o = ray.org;
    dvec3 cnt = orgn;

    double a = dot(d, d);
    double b = 2.0 * dot(d, o - cnt);
    double c = dot(o - cnt, o - cnt) - r * r;

    double det = b * b - 4.0 * a * c;

    if (det < 0.0)
    {
        return;
    }

    double t1 = (-b + sqrt(det)) / (2.0 * a);
    double t2 = (-b - sqrt(det)) / (2.0 * a);

    if (t1 <= t2 && t1 > 0.0)
    {
        intsc.t = t1;
        intsc.normal = normalize(o + t1 * d - orgn);
        intsc.mat = mat;
        return;
    }

    if (t2 > 0.0)
    {
        intsc.t = t2;
        intsc.normal = normalize(o + t2 * d - orgn);
        intsc.mat = mat;
        return;
    }
}

/**
 * [RTPlane::Intersect description]
 * @param  ray [description]
 */

void RTPlane::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t = -1.0;
    intsc.normal = dvec3(normal.x, normal.y, normal.z);

    dvec3 n = dvec3(normal.x, normal.y, normal.z);

    if (dot(n, ray.dir) == 0.0)
    {
        return;
    }

    double d = normal.w;
    intsc.t = (d - dot(n, ray.org)) / (dot(n, ray.dir));
    intsc.mat = mat;
}

/**
 * [RTScene::Intersect description]
 * @param  ray [description]
 */

void RTScene::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t = -1.0;
    double minDist = DBL_MAX;

    Intersection intsc2;
    plane.Intersect(ray, intsc2);

    if ((intsc2.t < minDist) && (intsc2.t > 0.0))
    {
        intsc = intsc2;
        minDist = intsc2.t;
    }

    for (uint32_t i = 0; i < spheres.size(); i++)
    {
        spheres[i].Intersect(ray, intsc2);

        if ((intsc2.t < minDist) && (intsc2.t > 0.0))
        {
            intsc = intsc2;
            minDist = intsc2.t;
        }
    }
}

/**
* [RTCamera::Init description]
* @param w     [description]
* @param h     [description]
* @param posIn [description]
* @param look  [description]
* @param fov   [description]
*/

void RTCamera::Init(
    uint32_t w,
    uint32_t h,
    dvec3 posIn,
    dvec3 look,
    double fov
)
{
    imageW = (double)w;
    imageH = (double)h;
    aspect = (double)w / (double)h;
    tanfov = tan(fov * pi<double>() / (360.0));

    pos = posIn;

    viewInv = lookAt(pos, look, dvec3(0.0, 0.0, 1.0));
    viewInv = inverse(viewInv);
}

/**
* [RTCamera::GenerateRay description]
* @param  pixel [description]
* @return       [description]
*/

Ray RTCamera::GenerateRay(dvec2 pixel)
{
    Ray ray;

    dvec3 dir;
    dir.x = (2.0 * pixel.x / imageW - 1.0) * tanfov * aspect;
    dir.y = (2.0 * pixel.y / imageH - 1.0) * tanfov;
    dir.z = -1.0;
    dir = normalize(dir);

    vec4 dir4 = vec4(dir, 1);
    dir4 = viewInv * dir4;

    dir.x = dir4.x;
    dir.y = dir4.y;
    dir.z = dir4.z;

    ray.dir = dir - pos;
    ray.org = pos;

    return ray;
}

/**
 * [RTMesh::Intersect description]
 * @param ray  [description]
 * @param intc [description]
 */

void RTMesh::Intersect(Ray ray, Intersection &intc)
{
    // Triangle intersection method using barycentric coordinates.
    // Adapter from the PBRT book.

    for (uint32_t i = 0; i < tris.size(); i++)
    {
        dvec3 p0 = pos[tris[i].x];
        dvec3 p1 = pos[tris[i].y];
        dvec3 p2 = pos[tris[i].z];

        dvec3 e1 = p1 - p0;
        dvec3 e2 = p2 - p0;
        dvec3 s = ray.org - p0;

        dvec3 s1 = cross(ray.dir, e2);
        dvec3 s2 = cross(s, e1);

        double a = 1.0 / dot(s1, e1);

        double t = a * dot(s2, e2);
        double b1 = a * dot(s1, s);
        double b2 = a * dot(s2, ray.dir);
    }
}