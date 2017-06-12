#include "raytrace.h"

DWORD WINAPI RayTracer::RTThreadFunc(LPVOID lpParam)
{
    RayTraceIn& rtIn = (RayTraceIn&)lpParam;
    RayTracer rt;

    return 0;
}