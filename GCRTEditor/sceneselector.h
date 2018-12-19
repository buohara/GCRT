#pragma once

#include <string>
#include <map>

#include "animationscene.h"
#include "terrainscene.h"
#include "envmapscene.h"

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