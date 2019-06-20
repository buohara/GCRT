#include "timeline.h"

double Timeline::Next()
{
    curTime += dt;
    return curTime;
}

void Timeline::Init(double tiIn, double tfIn, double fps)
{
    ti = tiIn;
    tf = tfIn;
    dt = 1.0 / fps;
    curTime = tiIn;
}

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