#include "rtrenderer.h"

CRITICAL_SECTION imageBlockCS;

/**
 * [GetMilliseconds description]
 * @return [description]
 */

long long GetMilliseconds()
{
    static LARGE_INTEGER frequency;
    static BOOL useQpc = QueryPerformanceFrequency(&frequency);
    
    if (useQpc) 
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / frequency.QuadPart;
    }
    else 
    {
        return GetTickCount();
    }
}

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

    dvec3 camPos = dvec3(5.0, -5.0, 3.0);
    dvec3 camLook = dvec3(0.0, 0.0, 2.0);
    
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
        xBlocks = 32;
        yBlocks = 24;
    }
    else
    {
        xBlocks = 24;
        yBlocks = 32;
    }

    uint32_t blockSizeX = imageW / xBlocks;
    uint32_t blockSizeY = imageH / yBlocks;

    for (uint32_t i = 0; i < xBlocks; i++)
    {
        for (uint32_t j = 0; j < yBlocks; j++)
        {
            Rect rect;

            rect.xmin = i * blockSizeX;
            rect.ymin = j * blockSizeY;

            if (i == xBlocks - 1)
            {
                rect.xmax = imageW - 1;
            }
            else
            {
                rect.xmax = (i + 1) * blockSizeX - 1;
            }

            if (j == yBlocks - 1)
            {
                rect.ymax = imageH - 1;
            }
            else
            {
                rect.ymax = (j + 1) * blockSizeY - 1;
            }
            
            imageBlocks.push_back(rect);
        }
    }

    InitializeCriticalSection(&imageBlockCS);

    numThreads = 16;

    for (uint32_t i = 0; i < numThreads; i++)
    {
        threadData[i].threadID = i;
        threadData[i].pScn = &scn;
        threadData[i].pImg = &image;
        threadData[i].imageW = imageW;
        threadData[i].imageH = imageH;
        threadData[i].pImageBlocks = &imageBlocks;
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
    
    uint32_t imageW = data.imageW;
    uint32_t imageH = data.imageH;
    RTScene &scn = *(data.pScn);
    vector<dvec3> &image = *(data.pImg);
    vector<Rect> &imageBlocks = *(data.pImageBlocks);

    Sampler sampler;

    SurfaceIntegrator integrator;
    integrator.GenerateSphereSamples(32);

    long long start = GetMilliseconds();

    while (1)
    {
        EnterCriticalSection(&imageBlockCS);

        if (imageBlocks.size() == 0)
        {
            LeaveCriticalSection(&imageBlockCS);

            long long stop = GetMilliseconds();
            char str[512];
            
            sprintf_s(
                str, 
                "Thread %u render time: %3.2fs\n", 
                data.threadID, 
                ((double)(stop - start) / 1000.0)
            );
            
            OutputDebugString(str);
            return 0;
        }

        Rect rect = imageBlocks.back();
        imageBlocks.pop_back();
        LeaveCriticalSection(&imageBlockCS);

        for (uint32_t y = rect.ymin; y <= rect.ymax; y++)
        {
            for (uint32_t x = rect.xmin; x <= rect.xmax; x++)
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
                        5
                    );

                    color;
                }

                color = 1.0 - glm::exp(-color);
                image[y * imageW + x] = color;
            }
        }
    }

    return 0;
}

/**
 * [RTRenderer::Render description]
 */

void RTRenderer::Render()
{
    long long start = GetMilliseconds();

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

    long long stop = GetMilliseconds();

    char str[512];
    sprintf_s(str, "Total render time: %3.2fs\n", ((double)(stop - start) / 1000.0));
    OutputDebugString(str);
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