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
    RTRenderer rndr;
    rndr.Init(512, 512);
    rndr.Render();
    rndr.SaveImage("C:/Users/beno.NVIDIA.COM/Desktop/test.jpg");
}