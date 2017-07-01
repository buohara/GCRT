#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

struct RTMaterial
{
    string name;

    double krefl;
    double ktrans;
    double kdiff;
    bool emissive;

    dvec3 kd;
    dvec3 ke;
};