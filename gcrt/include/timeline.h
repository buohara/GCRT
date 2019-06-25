#pragma once

#include <Windows.h>

struct Timeline
{
    double ti;
    double tf;
    double dt;
    double curTime;

    double Next();
    
    Timeline() : ti(0.0), tf(0.0), dt(1.0 / 24.0), curTime(0.0) {}
};

long long GetMilliseconds();