#include "rtrenderer.h"

CRITICAL_SECTION imageBlockCS;

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
 * - Load settings from file.
 * - Initialize output image.
 * - Configure filter parameters.
 * - Initialize render threads.
 * - Configure surface integrator settings.
 * - Load scene models/lights.
 * - Process virtual lights for scene.
 */

void RTRenderer::Init()
{
    LoadSettings("settings.txt");

    imageW = settings.imageW;
    imageH = settings.imageH;

    imageSamples.resize(imageW * imageH);
    outImage.resize(imageW * imageH);

    integrator.numVLightSets = settings.vLightSets;
    integrator.vLightSetSize = settings.vLightSetSize;
    integrator.curVLightSet  = 0;
    sampler.numSamples = settings.pixelSamples;
    
    filter.b = 0.33;
    filter.c = 0.33;
    filter.xw = 2.0;
    filter.yw = 2.0;

    scn.LoadDefaultScene(imageW, imageH);
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

    if (imageW > imageH)
    {
        xBlocks = settings.xBlocks;
        yBlocks = settings.yBlocks;
    }
    else
    {
        xBlocks = settings.yBlocks;
        yBlocks = settings.xBlocks;
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
}

/**
 * RTRenderer::InitThreads Prepare data for render threads.
 */

void RTRenderer::InitThreads()
{
    InitializeCriticalSection(&imageBlockCS);

#ifdef _DEBUG

    numThreads = 1;

#else

    numThreads = settings.numThreads;

#endif
    for (uint32_t i = 0; i < numThreads; i++)
    {
        threadData[i].threadID      = i;
        threadData[i].pScn          = &scn;
        threadData[i].pImgSamples   = &imageSamples;
        threadData[i].pSampler      = &sampler;
        threadData[i].imageW        = imageW;
        threadData[i].imageH        = imageH;
        threadData[i].pImageBlocks  = &imageBlocks;
        threadData[i].pIntegrator   = &integrator;
        threadData[i].camPathDepth  = settings.camPathDepth;
        threadData[i].dofSamples    = settings.dofSamples;
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

_declspec(thread) PathDebugData tls_pathDbgData = {};

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
    // Unpack thread params.

    ThreadData &data                = *((ThreadData*)(lpParam)); 
    uint32_t imageW                 = data.imageW;
    uint32_t imageH                 = data.imageH;
    uint32_t camPathDepth           = data.camPathDepth;
    RTScene &scn                    = *(data.pScn);
    vector<Rect> &imageBlocks       = *(data.pImageBlocks);
    Sampler &sampler                = *(data.pSampler);
    SurfaceIntegrator integrator    = *(data.pIntegrator);
    uint32_t dofSamples             = (data.dofSamples);
    double dofSamplesInv            = 1.0 / (double)(dofSamples + 1);

    vector<vector<Sample>> &imgSamples = *(data.pImgSamples);
    long long start = GetMilliseconds();

    memset(&tls_pathDbgData, 0, sizeof(PathDebugData));

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
                        Ray ray = (j == 0) ? primRay :
                            scn.cam.GenerateSecondaryRay(primRay, samples[i]);

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
    double curAnimTime = scn.tl.curTime;
    uint32_t frameNum = 0;

    while (curAnimTime <= scn.tl.tf)
    {
        GenerateImageBlocks();
        scn.UpdateAnimations(curAnimTime);

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
        curAnimTime = scn.tl.Next();
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
    for (uint32_t y = 0; y < imageH; y++)
    {
        for (uint32_t x = 0; x < imageW; x++)
        {
            vector<uvec2> filterBox;
            GetFilterBox((int)x, (int)y, settings.filterSize, filterBox);

            dvec3 weightedSamples = dvec3(0.0, 0.0, 0.0);
            double weightSum = 0.0;

            for (uint32_t k = 0; k < filterBox.size(); k++)
            {
                uint32_t pixelIdx = filterBox[k].y * imageW + filterBox[k].x;
                uint32_t numSamples = (uint32_t)imageSamples[pixelIdx].size();

                for (uint32_t l = 0; l < numSamples; l++)
                {
                    double evalX = imageSamples[pixelIdx][l].pos.x - (double)x;
                    double evalY = imageSamples[pixelIdx][l].pos.y - (double)y;
                    double weight = filter.Evaluate(evalX, evalY);

                    weightedSamples += weight * imageSamples[pixelIdx][l].color;
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
    uint32_t numPixels = imageW * imageH;
    for (uint32_t pixel = 0; pixel < numPixels; pixel++)
    {
        imageSamples[pixel].resize(0);
    }
}

/**
 * RTRenderer::LoadSettings Load ray tracer settings from settings file.
 * @param file Settings file path.
 */

void RTRenderer::LoadSettings(string file)
{
    ifstream fin;
    fin.open(file);

    string line;
    istringstream iss;

    uint32_t imageW;
    uint32_t imageH;
    uint32_t sphereSamples;
    uint32_t vLightSets;
    uint32_t vLightSetSize;
    uint32_t camPathDepth;
    uint32_t lightPathDepth;
    uint32_t pixelSamples;
    uint32_t filterSize;
    uint32_t dofSamples;
    uint32_t numThreads;
    uint32_t xBlocks;
    uint32_t yBlocks;
    bool scnFromFile;
    string scnFilePath;

    getline(fin, line);
    iss.str(line);
    iss >> imageW;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> imageH;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> sphereSamples;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> vLightSets;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> vLightSetSize;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> camPathDepth;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> lightPathDepth;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> pixelSamples;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> filterSize;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> dofSamples;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> numThreads;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> xBlocks;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> yBlocks;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> scnFromFile;
    iss.clear();

    getline(fin, line);
    iss.str(line);
    iss >> scnFilePath;
    iss.clear();

    settings.imageW         = imageW;
    settings.imageH         = imageH;
    settings.sphereSamples  = sphereSamples;
    settings.vLightSets     = vLightSets;
    settings.vLightSetSize  = vLightSetSize;
    settings.camPathDepth   = camPathDepth;
    settings.lightPathDepth = lightPathDepth;
    settings.pixelSamples   = pixelSamples;
    settings.filterSize     = filterSize;
    settings.dofSamples     = dofSamples;
    settings.numThreads     = numThreads;
    settings.xBlocks        = xBlocks;
    settings.yBlocks        = yBlocks;
    settings.scnFromFile    = scnFromFile;
    settings.scnFilePath    = scnFilePath;

    fin.close();
}