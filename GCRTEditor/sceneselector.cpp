#include "sceneselector.h"

map<string, PFNLAUNCH> launchTable =
{
    { "AnimationScene", AnimationScene::LaunchAnimationScene },
    { "TerrainScene", TerrainScene::LaunchTerrainScene }
};

void Launch(string &name, HINSTANCE hInstance)
{
    PFNLAUNCH pfn = launchTable[name];
    pfn(hInstance);
}