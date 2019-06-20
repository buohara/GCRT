#include "rtcamera.h"

/**
 * RTCamera::RTCamera - Camera default constructor.
 */

RTCamera::RTCamera() : 
    imageW(512), 
    imageH(512),
    pos(dvec3(8.0, -2.0, 3.0)),
    lookDir(dvec3(-8.0, 2.0, -3.0)),
    fov(45.0),
    aperture(0.5),
    focalDist(12.7),
    type(PERSPECTIVE) 
{
    aspect  = (double)imageW / (double)imageH;
    tanfov  = tan(fov * pi<double>() / (360.0));
    viewInv = lookAt(pos, lookDir, dvec3(0.0, 0.0, 1.0));
    viewInv = inverse(viewInv);
}

/**
 * RTCamera::RTCamera - Camera constructor.
 *
 * @param w     Image width.
 * @param h     Image height.
 * @param posIn Camera position.
 * @param look  Position camera is looking at.
 * @param fov   Camera field of view.
 * @param type  Camera type (ortho/perspective/etc)
 */

RTCamera::RTCamera(
    uint32_t w,
    uint32_t h,
    dvec3 pos,
    dvec3 look,
    double fov,
    double aperture,
    double focalDist,
    CamType type
) 
    : imageW(w), imageH(h), pos(pos), fov(fov), aperture(aperture), focalDist(focalDist), type(type)
{
    aspect      = (double)imageW / (double)imageH;
    tanfov      = tan(fov * pi<double>() / (360.0));
    viewInv     = lookAt(pos, look, dvec3(0.0, 0.0, 1.0));
    viewInv     = inverse(viewInv);
    lookDir     = look - pos;
}

/**
 * RTCamera::GeneratePrimaryRay - Generic routine for getting primary rays from camera.
 * Redirects to appropriate specialized function depending on camera type.
 *
 * @param pixel Image pixel to get primary ray for.
 *
 * @retrn Primary ray for input pixel.
 */

Ray RTCamera::GeneratePrimaryRay(dvec2 pixel)
{
    Ray ray = {};

    switch (type)
    {
    case PERSPECTIVE:

        ray = GeneratePrimaryRayPersp(pixel);
        break;

    case ORTHO:

        ray = GeneratePrimaryRayOrtho(pixel);
        break;

    default:

        break;
    }

    return ray;
}

/**
 * RTCamera::GeneratePrimaryRay - Generate camera rays for a simple ortho camera.
 *
 * @param pixel Image pixel to get primary ray for.
 *
 * @retrn Primary ray for input pixel.
 */

Ray RTCamera::GeneratePrimaryRayOrtho(dvec2 pixel)
{
    Ray ray;
    ray.dir = lookDir;

    ray.org.x = pos.x + 2.0 * (pixel.x) / (double)imageW - 1.0;
    ray.org.y = pos.y + 2.0 * (pixel.y) / (double)imageH - 1.0;

    return ray;
}

/**
 * RTCamera::GenerateRay - Generate camera rays for a simple perspective camera.
 *
 * @param  pixel Pixel to generate primary ray through.
 *
 * @return       Primary ray from desired pixels.
 */

Ray RTCamera::GeneratePrimaryRayPersp(dvec2 pixel)
{
    Ray ray;
    dvec3 dir;
    
    dir.x       = (2.0 * pixel.x / (double)imageW - 1.0) * tanfov * aspect;
    dir.y       = (2.0 * pixel.y / (double)imageH - 1.0) * tanfov;
    dir.z       = -1.0;
    dir         = normalize(dir);

    vec4 dir4   = vec4(dir, 1);
    dir4        = viewInv * dir4;

    dir.x       = dir4.x;
    dir.y       = dir4.y;
    dir.z       = dir4.z;

    ray.dir     = dir - pos;
    ray.org     = pos;

    return ray;
}

/**
 * RTCamera::GenerateSecondaryRay - Given a primary ray from an image pixel, generate a
 * perturbed secondary ray (used for depth of field).
 *
 * @param  primRay Primary ray to perturb.
 * @param  pixel   Pixel origin of primary ray.
 * @return         Newly generated secondary ray.
 */

Ray RTCamera::GenerateSecondaryRay(Ray primRay, dvec2 pixel)
{
    assert(type == ORTHO);

    Ray newRay;
    dvec3 dir;

    dir.x = (2.0 * pixel.x / (double)imageW - 1.0) * tanfov * aspect;
    dir.y = (2.0 * pixel.y / (double)imageH - 1.0) * tanfov;
    dir.z = -1.0;
    dir = normalize(dir);
    double t = -focalDist / dir.z;
    dvec3 intsc = primRay.org + t * primRay.dir;

    dvec3 e1 = normalize(dvec3(lookDir.z, 0.0, -lookDir.x));
    dvec3 e2 = normalize(cross(lookDir, e1));

    double theta = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;
    double r = aperture * (double)rand() / (double)RAND_MAX;

    newRay.org = primRay.org;
    newRay.org += r * cos(theta) * e1;
    newRay.org += r * sin(theta) * e2;
    newRay.dir = normalize(intsc - newRay.org);

    return newRay;
}