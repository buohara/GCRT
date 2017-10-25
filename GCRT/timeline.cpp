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