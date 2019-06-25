#include "rtrenderer.h"

// Globals.

mutex imageBlockMtx;
_declspec(thread) PathDebugData tls_pathDbgData = {};

vector<vector<Sample>> gImageSamples;
vector<Rect> gImageBlocks;
Sampler gPixelSampler;
uint32_t gTotalSamples;
uint32_t gSamplesProcessed = 0;

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
    uint32_t imageW = settings.imageW;
    uint32_t imageH = settings.imageH;

    gTotalSamples   = settings.pixelSamples * imageW * imageH;

    gImageSamples.resize(imageW * imageH);
    outImage.resize(imageW * imageH);

    gPixelSampler.numSamples    = settings.pixelSamples;
    
    filter.b    = 0.33;
    filter.c    = 0.33;
    filter.xw   = 2.0;
    filter.yw   = 2.0;

    Preprocess();
}

/**
 * RTRenderer::GenerateImageBlocks Break output image into tiles for threads
 * to render.
 */

void RTRenderer::GenerateImageBlocks()
{
    uint32_t xBlocks;
    uint32_t yBlocks;

    uint32_t imageW = settings.imageW;
    uint32_t imageH = settings.imageH;

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

            rect.xmax = (i == xBlocks - 1) ? rect.xmax = imageW - 1 : (i + 1) * blockSizeX - 1;
            rect.ymax = (j == yBlocks - 1) ? rect.ymax = imageH - 1 : (j + 1) * blockSizeY - 1;

            gImageBlocks.push_back(rect);
        }
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
 * @param  settings Renderer settings.
 * @paran  scn      Scene to be rendered.
 *
 * @return         Zero when no more thread work to consume.
 */

void RenderThreadFunc(RTRenderSettings &settings, RTScene &scn, uint32_t threadId, SurfaceIntegrator &integrator)
{
    // Unpack thread ID/global for this worker thread.
   
    Sampler &sampler                    = gPixelSampler;
    uint32_t imageW                     = settings.imageW;
    uint32_t imageH                     = settings.imageH;
    vector<Rect> &imageBlocks           = gImageBlocks;
    uint32_t dofSamples                 = settings.dofSamples;
    uint32_t camPathDepth               = settings.camPathDepth;
    vector<vector<Sample>> &imgSamples  = gImageSamples;

    double dofSamplesInv                = 1.0 / (double)(dofSamples + 1);
    long long start = GetMilliseconds();

    memset(&tls_pathDbgData, 0, sizeof(PathDebugData));

    while (1)
    {
        // Grab an image tile, exit if no more work left.

        imageBlockMtx.lock();

        if (imageBlocks.size() == 0)
        {
            imageBlockMtx.unlock();
            long long stop = GetMilliseconds();
            printf("Thread %d render time: %3.2fs\n", threadId,((double)(stop - start) / 1000.0));
            return;
        }

        Rect rect = imageBlocks.back();
        imageBlocks.pop_back();
        imageBlockMtx.unlock();

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

                        if (t > 0.0 && !intsc.isLight)
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

            imageBlockMtx.lock();
            gSamplesProcessed += (rect.xmax - rect.xmin) * settings.pixelSamples;
            cout << "\r" << gSamplesProcessed << " / " << gTotalSamples << " samples processed ...";
            imageBlockMtx.unlock();
        }
    }

    return;
}

/**
 * RTRenderer::Render Main render routine. For each animation timestamp:
 *
 * - Update scene animation. 
 * - Kick off threads to render image tiles.
 * - When threads are done, run filter over image samples to generate final image.
 * - Save final image to file.
 */

void RTRenderer::Render(RTScene &scn)
{
    long long start = GetMilliseconds();
    double curAnimTime = scn.tl.curTime;
    uint32_t frameNum = 0;

    printf("\n\nBeginning trace\n\n");

    SurfaceIntegrator integrator(settings.vLightSets, settings.vLightSetSize,
        settings.numLightSamples, settings.numBSDFSamples);

    while (curAnimTime <= scn.tl.tf)
    {
        GenerateImageBlocks();
        scn.UpdateAnimations(curAnimTime);

        for (uint32_t i = 0; i < settings.numThreads; i++) threads[i] = thread(RenderThreadFunc, settings, scn, i, integrator);
        for (uint32_t i = 0; i < settings.numThreads; i++) threads[i].join();

        FilterSamples();

        char frameSuffix[16];
        sprintf(frameSuffix, "%05d.jpg", frameNum);

        SaveImage(settings.outputPath + settings.frameFilePrefix + string(frameSuffix));

        frameNum++;
        ResetImageSamples();
        curAnimTime = scn.tl.Next();
    }

    long long stop = GetMilliseconds();
    printf("Total render time: %3.2fs\n", ((double)(stop - start) / 1000.0));
}

/**
 * RTRenderer::FilterSamples Apply simple filter box to generated image samples.
 */

void RTRenderer::FilterSamples()
{
    uint32_t imageW = settings.imageW;
    uint32_t imageH = settings.imageH;

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
    uint32_t imageW = settings.imageW;
    uint32_t imageH = settings.imageH;

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
    uint32_t imageW = settings.imageW;
    uint32_t imageH = settings.imageH;

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
    uint32_t imageW = settings.imageW;
    uint32_t imageH = settings.imageH;

    uint32_t numPixels = imageW * imageH;
    for (uint32_t pixel = 0; pixel < numPixels; pixel++) gImageSamples[pixel].resize(0);
}