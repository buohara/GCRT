#pragma once

#include <Windows.h>

struct Timeline
{
    double ti;
    double tf;
    double dt;
    double curTime;

    double Next();
    void Init(double tiIn, double tfIn, double fps);
};

long long GetMilliseconds();