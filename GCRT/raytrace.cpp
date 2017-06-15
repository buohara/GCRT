#include "raytrace.h"

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