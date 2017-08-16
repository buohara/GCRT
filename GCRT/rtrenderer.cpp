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

void RTRenderer::Init()
{
    LoadSettings("settings.txt");

    imageW = settings.imageW;
    imageH = settings.imageH;

    imageSamples.resize(imageW * imageH);
    outImage.resize(imageW * imageH);

    dvec3 camPos = dvec3(8.0, -2.0, 3.0);
    dvec3 camLook = dvec3(0.0, 0.0, 2.0);
    
    scn.cam.Init(
        imageW,
        imageH,
        camPos,
        camLook,
        45.0,
        0.5,
        12.7,
        settings.dofSamples
    );

    integrator.GenerateSphereSamples(settings.sphereSamples);
    sampler.numSamples = settings.pixelSamples;
    
    filter.b = 0.33;
    filter.c = 0.33;
    filter.xw = 2.0;
    filter.yw = 2.0;

    if (settings.scnFromFile == true)
    {
        scn.LoadScene(settings.scnFilePath);
    }
    else
    {
        scn.Init();
    }

    Preprocess(settings.lightPathDepth);
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

    InitializeCriticalSection(&imageBlockCS);

    numThreads = settings.numThreads;

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
    }
}

/**
 * [RTRenderer::Preprocess description]
 */

void RTRenderer::Preprocess(uint32_t lightPathDepth)
{
    GenerateVirtualLights(lightPathDepth);
}

/**
 * [RTRenderer::GenerateVirtualLights description]
 */

void RTRenderer::GenerateVirtualLights(uint32_t lightPathDepth)
{
    double bias = integrator.bias;

    for (uint32_t i = 0; i < scn.lights.size(); i++)
    {
        Ray ray;
        Intersection intsc;
        
        dmat4 scl = scale(dvec3(scn.lights[i].r));
        dvec3 emis = scn.mats[scn.lights[i].mat]->GetEmission(ray, intsc);
        emis;

        for (uint32_t k = 0; k < 1024; k++)
        {
            for (uint32_t j = 0; j < integrator.sphereSamples.size(); j++)
            {
                double theta = pi<double>() * (double)rand() / (double)RAND_MAX;
                double phi = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;

                dvec3 sample;

                sample.x = sin(theta) * cos(phi);
                sample.y = sin(theta) * sin(phi);
                sample.z = cos(theta);

                ray.org = scn.lights[i].orgn + sample;
                ray.dir = normalize(ray.org - scn.lights[i].orgn);

                scn.GenerateLightPath(
                    k,
                    ray,
                    lightPathDepth,
                    1,
                    emis
                );
            }
        }
    }
}

/**
 * [RenderThreadFunc description]
 * @param  lpParam [description]
 * @return         [description]
 */

DWORD WINAPI RenderThreadFunc(LPVOID lpParam)
{
    // Unpack thread params.

    ThreadData &data = *((ThreadData*)(lpParam)); 
    uint32_t imageW                 = data.imageW;
    uint32_t imageH                 = data.imageH;
    RTScene &scn                    = *(data.pScn);
    
    vector<vector<Sample>> &imgSamples 
                                    = *(data.pImgSamples);
    
    vector<Rect> &imageBlocks       = *(data.pImageBlocks);
    Sampler &sampler                = *(data.pSampler);
    SurfaceIntegrator integrator    = *(data.pIntegrator);
    double dofSamplesInv            = 1.0 / (double)(scn.cam.dofSamples + 1.0);

    long long start = GetMilliseconds();

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

                    for (uint32_t j = 0; j <= scn.cam.dofSamples; j++)
                    {
                        Ray ray = (j == 0) ? primRay :
                            scn.cam.GenerateSecondaryRay(primRay, samples[i]);

                        Intersection intsc;
                        scn.Intersect(ray, intsc);
                        double t = intsc.t;

                        if (t > 0.0)
                        {
                            color += integrator.SampleSurface(
                                ray,
                                scn,
                                intsc,
                                1,
                                5
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
    FilterSamples();

    long long stop = GetMilliseconds();

    char str[512];
    sprintf_s(str, "Total render time: %3.2fs\n", ((double)(stop - start) / 1000.0));
    OutputDebugString(str);
}

/**
 * [RTRenderer::FilterSamples description]
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
                    double evalX = (double)x - imageSamples[pixelIdx][l].pos.x;
                    double evalY = (double)y - imageSamples[pixelIdx][l].pos.y;
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
 * [RTRenderer::GetFilterBox description]
 * @param x         [description]
 * @param y         [description]
 * @param w         [description]
 * @param filterBox [description]
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
        &outImage[0]
    );

    ilSave(IL_JPG, fileName.c_str());
}

/**
 * [RTRenderer::LoadSettings description]
 * @param file [description]
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
    uint32_t pixelSamples;
    uint32_t filterSize;
    uint32_t dofSamples;
    uint32_t camPathDepth;
    uint32_t lightPathDepth;
    uint32_t numThreads;
    uint32_t xBlocks;
    uint32_t yBlocks;
    bool scnFromFile;
    string scnFilePath;

    getline(fin, line);
    getline(fin, line);
    iss.str(line);

    iss
        >> imageW
        >> imageH
        >> sphereSamples
        >> pixelSamples
        >> filterSize
        >> dofSamples
        >> camPathDepth
        >> lightPathDepth
        >> numThreads
        >> xBlocks
        >> yBlocks
        >> scnFromFile
        >> scnFilePath;

    settings.imageW         = imageW;
    settings.imageH         = imageH;
    settings.sphereSamples  = sphereSamples;
    settings.pixelSamples   = pixelSamples;
    settings.filterSize     = filterSize;
    settings.dofSamples     = dofSamples;
    settings.camPathDepth   = camPathDepth;
    settings.lightPathDepth = lightPathDepth;
    settings.numThreads     = numThreads;
    settings.xBlocks        = xBlocks;
    settings.yBlocks        = yBlocks;
    settings.scnFromFile    = scnFromFile;
    settings.scnFilePath    = scnFilePath;

    fin.close();
}