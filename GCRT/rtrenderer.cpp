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
    scn.Init();

    InitThreads();
}

/**
 * [RTRenderer::InitThreads description]
 */

void RTRenderer::InitThreads()
{
    uint32_t xBlocks;
    uint32_t yBlocks;

    if (imageW > imageH)
    {
        xBlocks = 8;
        yBlocks = 2;
    }
    else
    {
        xBlocks = 2;
        yBlocks = 8;
    }

    numThreads = xBlocks * yBlocks;
    uint32_t blockSizeX = imageW / xBlocks;
    uint32_t blockSizeY = imageH / yBlocks;
    uint32_t threadID = 0;

    for (uint32_t i = 0; i < xBlocks; i++)
    {
        for (uint32_t j = 0; j < yBlocks; j++)
        {
            threadData[threadID].rect.xmin = i * blockSizeX;
            threadData[threadID].rect.ymin = j * blockSizeY;

            if (i == xBlocks - 1)
            {
                threadData[threadID].rect.xmax = imageW - 1;
            }
            else
            {
                threadData[threadID].rect.xmax = (i + 1) * blockSizeX - 1;
            }

            if (j == yBlocks - 1)
            {
                threadData[threadID].rect.ymax = imageH - 1;
            }
            else
            {
                threadData[threadID].rect.ymax = (j + 1) * blockSizeY - 1;
            }

            threadID++;
        }
    }

    for (uint32_t i = 0; i < numThreads; i++)
    {
        threadData[i].pScn = &scn;
        threadData[i].pImg = &image;
        threadData[i].imageW = imageW;
        threadData[i].imageH = imageH;
    }
}

/**
 * [RenderThreadFunc description]
 * @param  lpParam [description]
 * @return         [description]
 */

DWORD WINAPI RenderThreadFunc(LPVOID lpParam)
{
    ThreadData &data = *((ThreadData*)(lpParam));
    
    uint32_t xmin = data.rect.xmin;
    uint32_t xmax = data.rect.xmax;
    uint32_t ymin = data.rect.ymin;
    uint32_t ymax = data.rect.ymax;
    uint32_t imageW = data.imageW;
    uint32_t imageH = data.imageH;

    RTScene &scn = *(data.pScn);
    vector<dvec3> &image = *(data.pImg);

    Sampler sampler;

    SurfaceIntegrator integrator;
    integrator.GenerateSphereSamples(8);

    for (uint32_t y = ymin; y <= ymax; y++)
    {
        for (uint32_t x = xmin; x <= xmax; x++)
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

    return 0;
}

/**
 * [RTRenderer::Render description]
 */

void RTRenderer::Render()
{
    for (uint32_t i = 0; i < numThreads; i++)
    {
        hThreadArray[i] = CreateThread(
            NULL,
            0,
            RenderThreadFunc,
            &threadData[i],
            0,
            NULL
        );
    }

    WaitForMultipleObjects(numThreads, hThreadArray, TRUE, INFINITE);

    /*for (uint32_t y = 0; y < imageH; y++)
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
    }*/
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