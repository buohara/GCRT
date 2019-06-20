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
    intsc.t = -1.0;
    double minDist = DBL_MAX;

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
 * RTCamera::Init Initialize camera properties.
 *
 * @param w     Image width.
 * @param h     Image height.
 * @param posIn Camera position.
 * @param look  Position camera is looking at.
 * @param fov   Camera field of view.
 */

void RTCamera::Init(
    uint32_t w,
    uint32_t h,
    dvec3 posIn,
    dvec3 look,
    double fov,
    double apertureIn,
    double focalDistIn
)
{
    imageW      = (double)w;
    imageH      = (double)h;
    aspect      = (double)w / (double)h;
    tanfov      = tan(fov * pi<double>() / (360.0));
    aperture    = apertureIn;
    focalDist   = focalDistIn;

    pos         = posIn;
    lookDir     = look - posIn;

    viewInv     = lookAt(pos, look, dvec3(0.0, 0.0, 1.0));
    viewInv     = inverse(viewInv);
}

/**
 * RTCamera::GenerateRay - Generate camera rays for a simple perspective camera.
 *
 * @param  pixel Pixel to generate primary ray through.
 *
 * @return       Primary ray from desired pixels.
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
    Ray newRay;
    dvec3 dir;

    dir.x       = (2.0 * pixel.x / imageW - 1.0) * tanfov * aspect;
    dir.y       = (2.0 * pixel.y / imageH - 1.0) * tanfov;
    dir.z       = -1.0;
    dir         = normalize(dir);
    double t    = -focalDist / dir.z;
    dvec3 intsc = primRay.org + t * primRay.dir;

    dvec3 e1 = normalize(dvec3(lookDir.z, 0.0, -lookDir.x));
    dvec3 e2 = normalize(cross(lookDir, e1));

    double theta    = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;
    double r        = aperture * (double)rand() / (double)RAND_MAX;
    
    newRay.org      = primRay.org;
    newRay.org      += r * cos(theta) * e1;
    newRay.org      += r * sin(theta) * e2;
    newRay.dir      = normalize(intsc - newRay.org);

    return newRay;
}

/**
 * [RTScene::UpdateAnimations description]
 */

void RTScene::UpdateAnimations(double t)
{
    for (auto& mesh : meshes)
    {
        mesh.UpdateAnimation(t);
    }
}

/**
 * [RTScene::Init description]
 */

void RTScene::LoadDefaultScene(uint32_t w, uint32_t h)
{
    dvec3 camPos = dvec3(8.0, -2.0, 3.0);
    dvec3 camLook = dvec3(0.0, 0.0, 2.0);

    cam.Init(
        w,
        h,
        camPos,
        camLook,
        45.0,
        0.5,
        12.7
    );

    InitDefaultMaterials();
    InitDefaultModels();
    tl.Init(0.0, 0.0, 24.0);
}

/**
 * [RTScene::InitDefaultMaterials description]
 */

void RTScene::InitDefaultMaterials()
{
    MirrorMaterial mirrorMat;
    mirrorMat.name = "Mirror";

    FresnelGlassMaterial glassMat;
    glassMat.name = "Glass";
    glassMat.etai = 1.0;
    glassMat.etat = 1.4;

    LambertMaterial greenMat;
    greenMat.name = "GreenMatte";
    greenMat.kd = { 0.1, 0.7, 0.2 };
    greenMat.Init(256, 16);

    LambertMaterial whiteMat;
    whiteMat.name = "WhiteMatte";
    whiteMat.kd = { 0.9, 0.8, 0.7 };
    whiteMat.Init(256, 16);

    LambertMaterial redMat;
    redMat.name = "RedMatte";
    redMat.kd = { 0.7, 0.1, 0.2 };
    redMat.Init(256, 16);

    BlinnMaterial metalMat;
    metalMat.name = "Metal";
    metalMat.ks = 15.0;

   /* mats[0]  = make_shared<MirrorMaterial>(mirrorMat);
    mats[1]  = make_shared<FresnelGlassMaterial>(glassMat);
    mats[2]  = make_shared<LambertMaterial>(greenMat);
    mats[3]  = make_shared<LambertMaterial>(redMat);
    mats[4]  = make_shared<LambertMaterial>(whiteMat);
    mats[5]  = make_shared<BlinnMaterial>(metalMat);*/
}

/**
 * [RTScene::InitDefaultModels description]
 */

void RTScene::InitDefaultModels()
{
    // Mirror sphere

    RTMesh mirrSph(SPHERE, MIRROR, 1.0, dvec3(-3.0, -3.0, 2.0));
    meshes.push_back(mirrSph);

    // White Lambert sphere

    RTMesh matteSphere(SPHERE, WHITE_MATTE, 1.0, dvec3(-3.0, 0.0, 2.0));
    meshes.push_back(matteSphere);

    // Burshed metal sphere

    RTMesh metalSphere(SPHERE, METAL, 1.0, dvec3(-3.0, 3.0, 2.0));
    meshes.push_back(metalSphere);

    // Cornell box

    RTMesh cornell(CORNELL);
    meshes.push_back(cornell);

    // Dragon

    RTMesh dragon(PLY, WHITE_MATTE, "../asset/models/dragon/dragon_vrip.ply");
    meshes.push_back(dragon);

    // Light source

    SphereLight lightSphWhite;
    lightSphWhite.Init(256, 16);
    
    lightSphWhite.pos       = { 0.0, 0.0, 3.5 };
    lightSphWhite.r         = 0.4;
    lightSphWhite.pwr       = { 20.0, 19.0, 18.0 };
    lights["WhiteSphere"]   = make_shared<SphereLight>(lightSphWhite);
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