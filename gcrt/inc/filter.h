#pragma once

#include "gcrt.h"

struct Filter
{
    double xw;
    double yw;
    double b;
    double c;

    double Evaluate(double x, double y);
    double Evaluate1D(double val);
};