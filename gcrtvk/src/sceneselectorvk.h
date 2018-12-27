#pragma once
#include <string>
#include <map>

#include "basicscene.h"

using namespace std;

typedef void(*PFNLAUNCH)(HINSTANCE);

struct SceneData
{
    string desc;
    PFNLAUNCH pfnLaunch;
};

void Launch(
    int argc,
    char** argv,
    HINSTANCE hInstance
);