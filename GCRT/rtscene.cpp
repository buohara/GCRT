#include "rtscene.h"

/**
 * [RTScene::Intersect description]
 * @param  ray [description]
 */

void RTScene::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t = -1.0;
    double minDist = DBL_MAX;

    Intersection intsc2;

    for (uint32_t i = 0; i < spheres.size(); i++)
    {
        spheres[i].Intersect(ray, intsc2);

        if ((intsc2.t < minDist) && (intsc2.t > 0.001))
        {
            intsc = intsc2;
            minDist = intsc2.t;
        }
    }

    for (uint32_t i = 0; i < meshes.size(); i++)
    {
        meshes[i]->Intersect(ray, intsc2);

        if ((intsc2.t < minDist) && (intsc2.t > 0.001))
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
    double fov,
    double apertureIn,
    double focalDistIn,
    uint32_t dofSamplesIn
)
{
    imageW = (double)w;
    imageH = (double)h;
    aspect = (double)w / (double)h;
    tanfov = tan(fov * pi<double>() / (360.0));
    aperture = apertureIn;
    focalDist = focalDistIn;
    dofSamples = dofSamplesIn;

    pos = posIn;
    lookDir = look - posIn;

    viewInv = lookAt(pos, look, dvec3(0.0, 0.0, 1.0));
    viewInv = inverse(viewInv);
}

/**
* [RTCamera::GenerateRay description]
* @param  pixel [description]
* @return       [description]
*/

Ray RTCamera::GeneratePrimaryRay(dvec2 pixel)
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

Ray RTCamera::GenerateSecondaryRay(Ray primRay, dvec2 pixel)
{
    Ray newRay;

    dvec3 dir;
    dir.x = (2.0 * pixel.x / imageW - 1.0) * tanfov * aspect;
    dir.y = (2.0 * pixel.y / imageH - 1.0) * tanfov;
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
    MirrorMaterial mirrorMat;
    mirrorMat.name = "Mirror";

    GlassMaterial glassMat;
    glassMat.name = "Glass";
    glassMat.etai = 1.0;
    glassMat.etat = 1.5;

    MatteMaterial greenMat;
    greenMat.name = "GreenMatte";
    greenMat.kd = dvec3(0.1, 0.7, 0.2);

    MatteMaterial whiteMat;
    whiteMat.name = "WhiteMatte";
    whiteMat.kd = dvec3(1.0, 0.9, 0.8);

    MatteMaterial redMat;
    redMat.name = "RedMatte";
    redMat.kd = vec3(0.7, 0.1, 0.2);

    LightMaterial lightMatBlue;
    lightMatBlue.name = "LightBlue";
    lightMatBlue.lightColor = dvec3(100.0, 200.0, 2000.0);

    LightMaterial lightMatRed;
    lightMatRed.name = "LightRed";
    lightMatRed.lightColor = dvec3(1000.0, 200.0, 100.0);

    LightMaterial lightMatWhite;
    lightMatWhite.name = "LightWhite";
    lightMatWhite.lightColor = dvec3(100.0, 90.0, 80.0);

    mats["Mirror"] = make_shared<MirrorMaterial>(mirrorMat);
    mats["Glass"] = make_shared<GlassMaterial>(glassMat);
    mats["GreenMatte"] = make_shared<MatteMaterial>(greenMat);
    mats["RedMatte"] = make_shared<MatteMaterial>(redMat);
    mats["WhiteMatte"] = make_shared<MatteMaterial>(whiteMat);
    mats["LightBlue"] = make_shared<LightMaterial>(lightMatBlue);
    mats["LightRed"] = make_shared<LightMaterial>(lightMatRed);
    mats["LightWhite"] = make_shared<LightMaterial>(lightMatWhite);
}

/**
 * [RTScene::InitModels description]
 */

void RTScene::InitModels()
{
    RTSphere redSph;
    redSph.orgn = dvec3(-10.0, 8.0, 2.0);
    redSph.r = 1.0;
    redSph.mat = "RedMatte";
    //spheres.push_back(redSph);

    RTSphere mirrSph;
    mirrSph.orgn = dvec3(-3.0, 2.0, 2.0);
    mirrSph.r = 1.0;
    mirrSph.mat = "Mirror";
    spheres.push_back(mirrSph);

    RTSphere glassSph;
    glassSph.orgn = dvec3(-2.0, 0.0, 2.5);
    glassSph.r = 2.0;
    glassSph.mat = "Glass";
    //spheres.push_back(glassSph);

    RTSphere lightSphBlue;
    lightSphBlue.orgn = dvec3(0.0, -2.0, 20.0);
    lightSphBlue.r = 3.0;
    lightSphBlue.mat = "LightBlue";
    //spheres.push_back(lightSphBlue);

    RTSphere lightSphRed;
    lightSphRed.orgn = dvec3(-2.0, 2.0, 15.0);
    lightSphRed.r = 1.0;
    lightSphRed.mat = "LightRed";
    //spheres.push_back(lightSphRed);

    RTSphere lightSphWhite;
    lightSphWhite.orgn = dvec3(-4.0, -2.0, 3.0);
    lightSphWhite.r = 0.2;
    lightSphWhite.mat = "LightWhite";
    spheres.push_back(lightSphWhite);

    shared_ptr<RTMesh> pMesh = make_shared<RTMesh>();
    pMesh->LoadModel("../asset/models/boblampclean/boblampclean.md5mesh");
    meshes.push_back(pMesh);

    shared_ptr<RTMesh> pCornellBox = make_shared<RTMesh>();
    pCornellBox->CreateCornellBox();
    meshes.push_back(pCornellBox);
}