#include "rtrenderer.h"
#include "testoctree.h"

extern RTRenderSettings gSettings;

vector<RTCLArg> settings
{
    {"-w", "Width of rendered image.", "512", GCRT_UINT, offsetof(RTRenderSettings, imageW)},
    {"-h", "Height of rendered image", "512", GCRT_UINT, offsetof(RTRenderSettings, imageH)},
    {"-vlightsets", "Number of virtual sets to use with global illumination", "16", GCRT_UINT, offsetof(RTRenderSettings, vLightSets)},
    {"-vlightsetsize", "Number of virtual lights per set when using global illumination.", "16", GCRT_UINT, offsetof(RTRenderSettings, vLightSetSize)},
    {"-campathdepth", "Max ray bounces to trace from camera.", "1", GCRT_UINT, offsetof(RTRenderSettings, camPathDepth)},
    {"-lightpathdepth", "Max ray bounces to trace from lights when using bidirectional path tracing.", "0", GCRT_UINT, offsetof(RTRenderSettings, lightPathDepth)},
    {"-pixelsamples", "Number of samples to trace per pixel.", "4", GCRT_UINT, offsetof(RTRenderSettings, pixelSamples)},
    {"-filtersize", "Size of filter to use for denoising.", "2", GCRT_UINT, offsetof(RTRenderSettings, filterSize)},
    {"-dofsamples", "Number of camera rays to use for depth of field.", "0", GCRT_UINT, offsetof(RTRenderSettings, dofSamples)},
    {"-numthreads", "Number of threads to use for multithreaded rendering.", "8", GCRT_UINT, offsetof(RTRenderSettings, numThreads)},
    {"-xblocks", "Number of tiles in image x-direction when multithreading.", "4", GCRT_UINT, offsetof(RTRenderSettings, xBlocks)},
    {"-yblocks", "Number of tiles in image y-direction when multithreading.", "4", GCRT_UINT, offsetof(RTRenderSettings, yBlocks)},
    {"-bsdfsamples", "Number of samples to generate from a material's BSDF after a ray intersects it.", "16", GCRT_UINT, offsetof(RTRenderSettings, numBSDFSamples)},
    {"-lightsamples", "Number of samples to generate toward each light source after a light intersects it.", "8", GCRT_UINT, offsetof(RTRenderSettings, numLightSamples)},
    {"-testoctree", "Test octree build and ray intersection performance.", "false", GCRT_BOOL, offsetof(RTRenderSettings, testOctree)}
};

/**
 * DisplayHelp - Loop through list of command line options above.
 */

void DisplayHelp()
{
    printf("GCRT Settings\n\n");
    for (auto &s : settings) printf("%s    %s\n", s.clName.c_str(), s.desc.c_str());
    printf("\n\n");
}

/**
 * InitDefaultSettings - Load default settings into renderer from table above.
 */

void InitDefaultSettings()
{
    uint32_t val;

    for (auto &s : settings)
    {
        switch (s.type)
        {
        case GCRT_UINT:

            val = atoi(s.defaultVal.c_str());
            *((uint32_t*)((uint8_t*)&gSettings + s.settingsOffset)) = val;
            break;

        default:
            break;
        }
    }
}

/**
 * ParseCommandLine - Parse command line arguments and set renderer settings.
 * 
 * @param args Vector of argument strings.
 */

void ParseCommandLine(vector<string> &args)
{
    uint32_t val;

    for (uint32_t i = 0; i < args.size();)
    {
        string arg = args[i];

        for (auto &s : settings)
        {
            if (arg == s.clName)
            {
                switch (s.type)
                {
                case GCRT_UINT:

                    if ((i + 1) >= (args.size()))
                    {
                        printf("No value specified for flag %s.\n", arg.c_str());
                        DisplayHelp();
                        exit(0);
                    }

                    val = atoi(args[i + 1].c_str());
                    *((uint32_t*)((uint8_t*)&gSettings + s.settingsOffset)) = val;
                    i += 2;

                    break;
                default:
                    break;
                }
            }
        }
    }
}

/**
 * main - Renderer main entry point. Parse command line arguments, kickoff renderer, and save
 * output images.
 * 
 * @param  argc Number of command line arguments.
 * @param  argv array of command line argument strings.
 * @return      NA.
 */

int main(int argc, char** argv)
{
    vector<string> args(argv + 1, argv + argc);
    InitDefaultSettings();
    ParseCommandLine(args);

    if (gSettings.testOctree)
    {
        TestOctree();
        return 0;
    }

    RTRenderer rndr;
    rndr.Init();
    rndr.Render();
    rndr.SaveImage("C:/Users/beno.NVIDIA.COM/Desktop/test.jpg");
}