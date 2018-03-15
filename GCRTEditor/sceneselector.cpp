#include "sceneselector.h"

map<string, PFNLAUNCH> launchTable =
{
    { "AnimationScene", AnimationScene::LaunchAnimationScene }
};

void Launch(string &name, HINSTANCE hInstance)
{
    PFNLAUNCH pfn = launchTable[name];
    pfn(hInstance);
}