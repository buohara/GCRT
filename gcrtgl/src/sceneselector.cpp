#include "sceneselector.h"

map<string, SceneData> launchTable =
{
	{ "Animation",				{ "Skeletal animation sample.", AnimationScene::LaunchAnimationScene } },
	{ "EnvironmentMapping",		{ "Environment mapping sample.", EnvMapScene::LaunchEnvMapScene } },
	{ "Terrain",				{ "Scene demonstrating Perlin noise terrain.", TerrainScene::LaunchTerrainScene } }
};

/**
 * DisplayHelpAndExit Display help and list of available scenes, then exit.
 */

static void DisplayHelpAndExit()
{
	printf("GCRT Editor\n\n");
	printf("Usage:\n\n");
	printf("%-32s %s\n", "GCRTEditr.exe", "No options scecified. Show this menu.");
	printf("%-32s %s\n\n", "GCRTEditr.exe <Scene>", "Render a named scene. See options below");
	printf("Available Scenes:\n\n");

	for (auto& scn : launchTable)
	{
		printf("\t%-32s %s\n", scn.first.c_str(), scn.second.desc.c_str());
	}

	exit(0);
}

/**
 * Launch Parse command params and launch scene specified by user.
 *
 * @param argc      Number of command line arguments.
 * @param argv      Vector of arguments.
 * @param hInstance Process handle.
 */

void Launch(int argc, char** argv, HINSTANCE hInstance)
{
	if (argc == 1)
	{
		DisplayHelpAndExit();
	}

	vector<string> args(argv + 1, argv + argc);
	string name = args[0];

	if (launchTable.count(name) == 0)
	{
		printf("Unrecognized flag/scene: %s\n\n", name.c_str());
		DisplayHelpAndExit();
	}

    PFNLAUNCH pfn = launchTable[name].pfnLaunch;
    pfn(hInstance);
}