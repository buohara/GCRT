#include "rtrenderer.h"

// Globals.

CRITICAL_SECTION imageBlockCS;
_declspec(thread) PathDebugData tls_pathDbgData = {};

RTRenderSettings gSettings = {};
RTScene gScn;
vector<vector<Sample>> gImageSamples;
vector<Rect> gImageBlocks;
SurfaceIntegrator gIntegrator;
Sampler gPixelSampler;
uint32_t gTotalSamples;
uint32_t gSamplesProcessed = 0;

/**
 * GetMilliseconds Get timestamp in milliseconds since beginning of clock epoch.
 * @return Timestamp in milliseconds.
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
 * RTRenderer::Init Initialize the raytrace renderer.
 *
 * - Initialize output image.
 * - Configure filter parameters.
 * - Initialize render threads.
 * - Configure surface integrator settings.
 * - Load scene models/lights.
 * - Process virtual lights for scene.
 */

void RTRenderer::Init()
{
    uint32_t imageW = gSettings.imageW;
    uint32_t imageH = gSettings.imageH;

    gTotalSamples = gSettings.pixelSamples * imageW * imageH;

    gImageSamples.resize(imageW * imageH);
    outImage.resize(imageW * imageH);

    integrator.numVLightSets    = gSettings.vLightSets;
    integrator.vLightSetSize    = gSettings.vLightSetSize;
    integrator.curVLightSet     = 0;
    gPixelSampler.numSamples    = gSettings.pixelSamples;
    
    filter.b    = 0.33;
    filter.c    = 0.33;
    filter.xw   = 2.0;
    filter.yw   = 2.0;

    gScn.LoadDefaultScene(imageW, imageH);
    Preprocess();
    InitThreads();
}

/**
 * RTRenderer::GenerateImageBlocks Break output image into tiles for threads
 * to render.
 */

void RTRenderer::GenerateImageBlocks()
{
    uint32_t xBlocks;
    uint32_t yBlocks;

    uint32_t imageW = gSettings.imageW;
    uint32_t imageH = gSettings.imageH;

    if (imageW > imageH)
    {
        xBlocks = gSettings.xBlocks;
        yBlocks = gSettings.yBlocks;
    }
    else
    {
        xBlocks = gSettings.yBlocks;
        yBlocks = gSettings.xBlocks;
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

            gImageBlocks.push_back(rect);
        }
    }
}

/**
 * RTRenderer::InitThreads Prepare data for render threads.
 */

void RTRenderer::InitThreads()
{
    InitializeCriticalSection(&imageBlockCS);

    numThreads = gSettings.numThreads;

    for (uint32_t i = 0; i < numThreads; i++)
    {
        threadData[i].threadID      = i;
    }
}

/**
 * RTRenderer::Preprocess Do any preprocessing on scene before rendering.
 */

void RTRenderer::Preprocess()
{
    GenerateVirtualLights();
}

/**
 * RTRenderer::GenerateVirtualLights Shoot rays from light sources and deposit virtual
 * lights around scene for bidirectional path tracing.
 */

void RTRenderer::GenerateVirtualLights()
{
    return;

    //double bias = integrator.bias;
    //scn.vLights.resize(settings.vLightSets);

    //for (uint32_t i = 0; i < scn.lights.size(); i++)
    //{
    //    Ray ray;
    //    Intersection intsc;

    //    dmat4 scl = scale(dvec3(scn.lights[i].r));
    //    dvec3 emis = scn.mats[scn.lights[i].mat]->GetEmission(ray, intsc);
    //    emis;

    //    for (uint32_t k = 0; k < settings.vLightSets; k++)
    //    {
    //        for (uint32_t j = 0; j < settings.vLightSetSize; j++)
    //        {
    //            //double theta = pi<double>() * (double)rand() / (double)RAND_MAX;
    //            double theta = acos(2.0 * (double)rand() / (double)RAND_MAX - 1.0);
    //            double phi = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;

    //            dvec3 sample;

    //            sample.x = sin(theta) * cos(phi);
    //            sample.y = sin(theta) * sin(phi);
    //            sample.z = cos(theta);

    //            ray.org = scn.lights[i].orgn + sample;
    //            ray.dir = normalize(ray.org - scn.lights[i].orgn);

    //            scn.GenerateLightPath(
    //                k,
    //                ray,
    //                settings.lightPathDepth,
    //                1,
    //                emis
    //            );
    //        }
    //    }
    //}
}

/**
 * RenderThreadFunc Render thread routine. Grab image tiles from queue
 * and ray trace pixels in that tile until no more tiles.
 *
 * @param  lpParam Render thread data.
 *
 * @return         Zero when no more thread work to consume.
 */

DWORD WINAPI RenderThreadFunc(LPVOID lpParam)
{
    // Unpack thread ID/global for this worker thread.

    ThreadData &data                    = *((ThreadData*)(lpParam));   
    Sampler &sampler                    = gPixelSampler;
    SurfaceIntegrator integrator        = gIntegrator;
    uint32_t imageW                     = gSettings.imageW;
    uint32_t imageH                     = gSettings.imageH;
    vector<Rect> &imageBlocks           = gImageBlocks;
    uint32_t dofSamples                 = gSettings.dofSamples;
    uint32_t camPathDepth               = gSettings.camPathDepth;
    vector<vector<Sample>> &imgSamples  = gImageSamples;
    RTScene &scn                        = gScn;

    double dofSamplesInv            = 1.0 / (double)(dofSamples + 1);

    long long start = GetMilliseconds();

    memset(&tls_pathDbgData, 0, sizeof(PathDebugData));
    tls_pathDbgData.intscMaterials[0] = CAMERA;

    while (1)
    {
        // Grab an image tile, exit if no more work left.

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

        // Sample (ray trace) pixels in current tile.

        for (uint32_t y = rect.ymin; y <= rect.ymax; y++)
        {
            for (uint32_t x = rect.xmin; x <= rect.xmax; x++)
            {
                vector<dvec2> samples;
                sampler.GenerateSamples(x, y, samples);         

                for (uint32_t i = 0; i < sampler.numSamples; i++)
                {
                    Ray primRay = scn.cam.GeneratePrimaryRay(samples[i]);
                    dvec3 color = dvec3(0.0, 0.0, 0.0);

                    for (uint32_t j = 0; j <= dofSamples; j++)
                    {
                        Ray ray = (j == 0) ? primRay : scn.cam.GenerateSecondaryRay(primRay, samples[i]);

                        Intersection intsc;
                        scn.Intersect(ray, intsc);
                        double t = intsc.t;

                        if (t > 0.0 && intsc.mat != 6)
                        {
                            color += integrator.SampleSurface(
                                ray,
                                scn,
                                intsc,
                                1,
                                camPathDepth
                            ) * dofSamplesInv;
                        }
                    }

                    Sample sample;
                    sample.pos = samples[i];
                    sample.color = color;

                    imgSamples[y * imageW + x].push_back(sample);
                }
            }

            EnterCriticalSection(&imageBlockCS);
            gSamplesProcessed += (rect.xmax - rect.xmin) * gSettings.pixelSamples;
            cout << "\r" << gSamplesProcessed << " / " << gTotalSamples << " samples processed ...";
            LeaveCriticalSection(&imageBlockCS);
        }
    }

    return 0;
}

/**
 * RTRenderer::Render Main render routine. For each animation timestamp:
 *
 * - Update scene animation. 
 * - Kick off threads to render image tiles.
 * - When threads are done, run filter over image samples to generate final image.
 * - Save final image to file.
 */

void RTRenderer::Render()
{
    long long start = GetMilliseconds();
    double curAnimTime = gScn.tl.curTime;
    uint32_t frameNum = 0;

    cout << "\n\nBeginning trace\n" << endl;

    while (curAnimTime <= gScn.tl.tf)
    {
        GenerateImageBlocks();
        gScn.UpdateAnimations(curAnimTime);

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
        FilterSamples();

        stringstream ss;
        ss << "C:/Users/beno.NVIDIA.COM/Desktop/Frames/Frame" << std::setfill('0')
            << std::setw(5) << frameNum << ".jpg";

        string fileName = ss.str();
        SaveImage(fileName);

        frameNum++;
        ResetImageSamples();
        curAnimTime = gScn.tl.Next();
    }

    long long stop = GetMilliseconds();

    char str[512];
    sprintf_s(str, "Total render time: %3.2fs\n", ((double)(stop - start) / 1000.0));
    OutputDebugString(str);
}

/**
 * RTRenderer::FilterSamples Apply simple filter box to generated image samples.
 */

void RTRenderer::FilterSamples()
{
    uint32_t imageW = gSettings.imageW;
    uint32_t imageH = gSettings.imageH;

    for (uint32_t y = 0; y < imageH; y++)
    {
        for (uint32_t x = 0; x < imageW; x++)
        {
            vector<uvec2> filterBox;
            GetFilterBox((int)x, (int)y, gSettings.filterSize, filterBox);

            dvec3 weightedSamples = dvec3(0.0, 0.0, 0.0);
            double weightSum = 0.0;

            for (uint32_t k = 0; k < filterBox.size(); k++)
            {
                uint32_t pixelIdx = filterBox[k].y * imageW + filterBox[k].x;
                uint32_t numSamples = (uint32_t)gImageSamples[pixelIdx].size();

                for (uint32_t l = 0; l < numSamples; l++)
                {
                    double evalX = gImageSamples[pixelIdx][l].pos.x - (double)x;
                    double evalY = gImageSamples[pixelIdx][l].pos.y - (double)y;
                    double weight = filter.Evaluate(evalX, evalY);

                    weightedSamples += weight * gImageSamples[pixelIdx][l].color;
                    weightSum += weight;
                }
            }

            outImage[y * imageW + x] = 1.0 - exp(-weightedSamples / weightSum);
        }
    }
}

/**
 * RTRenderer::GetFilterBox 
 *
 * @param x         Pixel x position.
 * @param y         Pixel y position.
 * @param w         Filter box width.
 * @param filterBox List of neighbor pixels to filter for current (x,y) pixel.
 */

void RTRenderer::GetFilterBox(int x, int y, int w, vector<uvec2> &filterBox)
{
    uint32_t imageW = gSettings.imageW;
    uint32_t imageH = gSettings.imageH;

    for (int i = -w; i <= w; i++)
    {
        for (int j = -w; j <= w; j++)
        {
            int fx = x + i;
            int fy = y + j;

            if (fx < 0 || 
                fx >= (int)imageW ||
                fy < 0 ||
                fy >= (int)imageH)
            {
                continue;
            }

            uvec2 f;
            f.x = (uint32_t)fx;
            f.y = (uint32_t)fy;

            filterBox.push_back(f);
        }
    }
}

/**
 * RTRenderer::SaveImage Save a final image to file.
 * @param fileName Output image file name.
 */

void RTRenderer::SaveImage(string fileName)
{
    uint32_t imageW = gSettings.imageW;
    uint32_t imageH = gSettings.imageH;

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
        &outImage[0]
    );

    ilSave(IL_JPG, fileName.c_str());
}

/**
 * RTRenderer::ResetImageSamples Clear all samples generated for image from previous
 * ray tracing passes.
 */

void RTRenderer::ResetImageSamples()
{
    uint32_t imageW = gSettings.imageW;
    uint32_t imageH = gSettings.imageH;

    uint32_t numPixels = imageW * imageH;
    for (uint32_t pixel = 0; pixel < numPixels; pixel++)
    {
        gImageSamples[pixel].resize(0);
    }
}