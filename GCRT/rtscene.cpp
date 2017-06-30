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

    for (uint32_t i = 0; i < meshes.size(); i++)
    {
        meshes[i].Intersect(ray, intsc2);

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

void RTMesh::Intersect(Ray ray, Intersection &intsc)
{
    // Triangle intersection method using barycentric coordinates.
    // Adapted from the PBRT book.

    intsc.t = DBL_MAX;

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

        if (b1 < 0.0 || b2 < 0.0 || (b1 + b2 > 1.0))
        {
            continue;
        }

        if (t > 0.0 && t < intsc.t)
        {
            intsc.t = t;
            intsc.mat = mat;
            intsc.normal = (norm[tris[i].x] + norm[tris[i].y] + norm[tris[i].z]) / 3.0;    
        }
    }
}

/**
 * [RTScene::Init description]
 */

void RTScene::Init()
{
    InitMaterials();
    InitModels();
}

/**
 * [RTScene::InitMaterials description]
 */

void RTScene::InitMaterials()
{
    RTMaterial mirrorMat;
    mirrorMat.name = "Mirror";
    mirrorMat.emissive = false;
    mirrorMat.kdiff = 0.0;
    mirrorMat.krefl = 1.0;
    mirrorMat.ktrans = 0.0;

    RTMaterial glassMat;
    glassMat.name = "Glass";
    glassMat.emissive = false;
    glassMat.kdiff = 0.0;
    glassMat.krefl = 0.5;
    glassMat.ktrans = 0.5;

    RTMaterial greenMat;
    greenMat.name = "GreenMatte";
    greenMat.kd = dvec3(0.1, 0.7, 0.2);
    greenMat.emissive = false;
    greenMat.kdiff = 1.0;
    greenMat.krefl = 0.0;
    greenMat.ktrans = 0.0;

    RTMaterial redMat;
    redMat.name = "RedMatte";
    redMat.emissive = false;
    redMat.kd = dvec3(0.7, 0.1, 0.2);
    redMat.kdiff = 1.0;
    redMat.krefl = 0.0;
    redMat.ktrans = 0.0;

    RTMaterial lightMatBlue;
    lightMatBlue.name = "LightBlue";
    lightMatBlue.emissive = true;
    lightMatBlue.ke = dvec3(100.0, 200.0, 2000.0);
    lightMatBlue.kdiff = 0.0;
    lightMatBlue.krefl = 0.0;
    lightMatBlue.ktrans = 0.0;

    RTMaterial lightMatRed;
    lightMatRed.name = "LightRed";
    lightMatRed.emissive = true;
    lightMatRed.ke = dvec3(2000.0, 200.0, 100.0);
    lightMatRed.kdiff = 0.0;
    lightMatRed.krefl = 0.0;
    lightMatRed.ktrans = 0.0;

    mats["Mirror"] = make_shared<RTMaterial>(mirrorMat);
    mats["Glass"] = make_shared<RTMaterial>(glassMat);
    mats["GreenMatte"] = make_shared<RTMaterial>(greenMat);
    mats["RedMatte"] = make_shared<RTMaterial>(redMat);
    mats["LightBlue"] = make_shared<RTMaterial>(lightMatBlue);
    mats["LightRed"] = make_shared<RTMaterial>(lightMatRed);
}

/**
 * [RTScene::InitModels description]
 */

void RTScene::InitModels()
{
    plane.normal = vec4(0.0, 0.0, 1.0, 0.0);
    plane.mat = mats["GreenMatte"];

    RTSphere redSph;
    redSph.orgn = dvec3(-7.0, 0.0, 2.0);
    redSph.r = 1.0;
    redSph.mat = mats["RedMatte"];
    spheres.push_back(redSph);

    RTSphere mirrSph;
    mirrSph.orgn = dvec3(0.0, -2.0, 2.0);
    mirrSph.r = 1.0;
    mirrSph.mat = mats["Mirror"];
    spheres.push_back(mirrSph);

    RTSphere glassSph;
    glassSph.orgn = dvec3(-2.0, 2.0, 3.0);
    glassSph.r = 1.0;
    glassSph.mat = mats["Glass"];
    spheres.push_back(glassSph);

    RTSphere lightSphBlue;
    lightSphBlue.orgn = dvec3(0.0, -5.0, 15.0);
    lightSphBlue.r = 1.0;
    lightSphBlue.mat = mats["LightBlue"];;
    spheres.push_back(lightSphBlue);

    RTSphere lightSphRed;
    lightSphRed.orgn = dvec3(-2.0, 0.0, 15.0);
    lightSphRed.r = 1.0;
    lightSphRed.mat = mats["LightRed"];;
    spheres.push_back(lightSphRed);

    RTMesh mesh;
    mesh.pos.push_back(dvec3(1.0, 0.0, 0.0));
    mesh.pos.push_back(dvec3(0.0, 0.0, 1.0));
    mesh.pos.push_back(dvec3(-1.0, 0.0, 0.0));

    mesh.norm.push_back(dvec3(0.0, 1.0, 0.0));
    mesh.norm.push_back(dvec3(0.0, 1.0, 0.0));
    mesh.norm.push_back(dvec3(0.0, 1.0, 0.0));

    mesh.tris.push_back(uvec3(0, 1, 2));
    mesh.mat = mats["RedMatte"];

    //meshes.push_back(mesh);
}