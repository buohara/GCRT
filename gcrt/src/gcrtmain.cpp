#include "testscene.h"

typedef void(*pfnTestScene)(void);

struct TestScene
{
    pfnTestScene testScene;
    string desc;
};

map<string, TestScene> scenes =
{
    { "TestOctree", { TestOctree, "Scene for testing octree ray intersection performance." } }
};

/**
 * DisplayScenesAndExit - List available test scenes and exit driver.
 */

void DisplayScenesAndExit()
{
    printf("GCRT usage: GRCT.exe <scene name>\n\n");
    printf("Available scenes:\n");

    for (auto& scene : scenes)
    {
        printf("%s - %s\n", scene.first.c_str(), scene.second.desc.c_str());
    }

    exit(0);
}

/**
 * main - Renderer main entry point. Parse command line arguments, kickoff rendering scene.
 * 
 * @param  argc Number of command line arguments.
 * @param  argv array of command line argument strings.
 * @return      NA.
 */

int main(int argc, char** argv)
{
    vector<string> args(argv + 1, argv + argc);
    
    if (scenes.count(args[0]) == 0) DisplayScenesAndExit();

    pfnTestScene pfnScene = scenes[args[0]].testScene;
    pfnScene();

    return 0;
}