#include "rtrenderer.h"

/**
 * [RTRenderer::Init description]
 * @param w [description]
 * @param h [description]
 */

void RTRenderer::Init(uint32_t w, uint32_t h)
{
    imageW = w;
    imageH = h;

    image.resize(imageW * imageH);

    dvec3 camPos = dvec3(10.0, 10.0, 10.0);
    dvec3 camLook = dvec3(0.0, 0.0, 3.0);
    scn.cam.Init(imageW, imageH, camPos, camLook, 75.0);
    
    integrator.GenerateSamplePoints(32);

    InitScene();
}

/**
 * [RTRenderer::InitScene description]
 */

void RTRenderer::InitScene()
{
    RTMaterial mirrorMat;
    mirrorMat.name = "Mirror";
    mirrorMat.type = "Mirror";

    RTMaterial glassMat;
    glassMat.name = "Glass";
    glassMat.type = "Glass";

    RTMaterial greenMat;
    greenMat.name = "GreenMatte";
    greenMat.kd = dvec3(0.1, 0.7, 0.2);
    greenMat.type = "Matte";
    greenMat.maxAlpha = 1.0;

    RTMaterial redMat;
    redMat.name = "RedMatte";
    redMat.kd = dvec3(0.7, 0.1, 0.2);
    redMat.type = "Matte";
    greenMat.maxAlpha = 1.0;

    RTMaterial lightMatBlue;
    lightMatBlue.name = "LightBlue";
    lightMatBlue.type = "Light";
    lightMatBlue.lightColor = dvec3(100.0, 200.0, 1000.0);

    RTMaterial lightMatRed;
    lightMatRed.name = "Light";
    lightMatRed.type = "Light";
    lightMatRed.lightColor = dvec3(1000.0, 200.0, 100.0);

    scn.plane.normal = vec4(0.0, 0.0, 1.0, 0.0);
    scn.plane.mat = greenMat;

    RTSphere redSph;
    redSph.orgn = dvec3(-7.0, 0.0, 2.0);
    redSph.r = 1.0;
    redSph.mat = redMat;
    scn.spheres.push_back(redSph);

    RTSphere mirrSph;
    mirrSph.orgn = dvec3(0.0, 0.0, 2.0);
    mirrSph.r = 1.0;
    mirrSph.mat = mirrorMat;
    scn.spheres.push_back(mirrSph);

    RTSphere glassSph;
    glassSph.orgn = dvec3(-2.0, 2.0, 3.0);
    glassSph.r = 1.0;
    glassSph.mat = glassMat;
    scn.spheres.push_back(glassSph);

    RTSphere lightSphBlue;
    lightSphBlue.orgn = dvec3(0.0, 2.0, 15.0);
    lightSphBlue.r = 1.0;
    lightSphBlue.mat = lightMatBlue;
    scn.spheres.push_back(lightSphBlue);

    RTSphere lightSphRed;
    lightSphRed.orgn = dvec3(-2.0, -2.0, 15.0);
    lightSphRed.r = 1.0;
    lightSphRed.mat = lightMatRed;
    //scn.spheres.push_back(lightSphRed);
}

/**
 * [RTRenderer::Render description]
 */

void RTRenderer::Render()
{
    for (uint32_t y = 0; y < imageH; y++)
    {
        for (uint32_t x = 0; x < imageW; x++)
        {
            vector<dvec2> samples;
            samples.resize(1);
            sampler.GenerateSamples(1, x, y, samples);
            Ray ray = scn.cam.GenerateRay(samples[0]);
         
            Intersection intsc;
            scn.Intersect(ray, intsc);
            dvec3 color = dvec3(0.0, 0.0, 0.0);

            if (intsc.t > 0.0)
            {
                color = integrator.SampleSurface(
                    ray,
                    scn,
                    intsc,
                    1,
                    4
                );
            }

            color = dvec3(1.0) - glm::exp(-color);
            image[y * imageW + x] = color;
        }
    }
}

/**
 * [RTRenderer::SaveImage description]
 * @param fileName [description]
 */

void RTRenderer::SaveImage(string fileName)
{
    ilInit();
    ilEnable(IL_FILE_OVERWRITE);

    ILuint imgID = ilGenImage();
    ilTexImage(
        imageW,
        imageH,
        1,
        3,
        IL_RGB,
        IL_DOUBLE,
        &image[0]
    );

    ilSave(IL_JPG, fileName.c_str());
}