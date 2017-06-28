#include "raytrace.h"
#include "rtrenderer.h"

/**
 * [RayTracer::RTThreadFunc description]
 * @param  lpParam [description]
 * @return         [description]
 */

DWORD WINAPI RayTracer::RTThreadFunc(LPVOID lpParam)
{
    RayTraceIn& rtIn = (RayTraceIn&)lpParam;
    return 0;
}

void RTMain()
{
    uint32_t w;
    uint32_t h;

#ifdef _DEBUG
    w = 256;
    h = 256;
#else
    w = 1080;
    h = 1920;
#endif

    RTRenderer rndr;
    rndr.Init(w, h);
    rndr.Render();
    rndr.SaveImage("C:/Users/beno.NVIDIA.COM/Desktop/test.jpg");
}