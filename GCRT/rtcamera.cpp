#include "rtcamera.h"

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
    aspect = (double) w / (double) h;
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