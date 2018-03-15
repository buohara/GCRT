#pragma once

#include <string>
#include <map>

#include "animationscene.h"

using namespace std;

typedef void(*PFNLAUNCH)(HINSTANCE);

void Launch(
    string &sceneName, 
    HINSTANCE hInstance
);