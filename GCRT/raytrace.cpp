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
    rndr.Init(1920, 1080);
    rndr.Render();
    rndr.SaveImage("C:/Users/SLI/Desktop/test.jpg");
}