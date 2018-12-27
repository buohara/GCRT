#include "envmapscene.h"

extern RenderSettings g_settings;
extern bool gResized;
extern Scene g_scn;

/**
 * EnvMapScene::LaunchEnvMapScene Create scene instance and launch it.
 * @param hInstance Process handle. Need to pass to renderer for creating GL context.
 */

void EnvMapScene::LaunchEnvMapScene(HINSTANCE hInstance)
{
	EnvMapScene animScn;
	animScn.Init(hInstance);
	animScn.Render();
}

/**
 * EnvMapScene::Init Initialize scene's renderer.
 * @param hInstance Process handle to create GL context.
 */

void EnvMapScene::Init(HINSTANCE hInstance)
{
    g_settings.loadSceneFromFile    = false;
    g_settings.msaaSamples          = 4;
    g_settings.useBloom             = false;
    g_settings.useDOF               = false;
    g_settings.winH                 = 1080;
    g_settings.winW                 = 1920;
    g_settings.wireFrame            = false;
    g_settings.useSkyBox            = true;

	rndr.CreateRenderWindow(
		hInstance,
		"EnvMapScene"
	);

	rndr.CreateGLContext();
	rndr.Init();

    g_scn.SetSkyTex(string("F:/GCRT/asset/skypano.jpg"),
        ImgLoader::LoadTexture(string("F:/GCRT/asset/skypano.jpg")));

	MainPass mainPass;
	mainPass.Init(0, true);

	rndr.passes["MainPass"] = make_shared<MainPass>(mainPass);

    LoadScene();
}

/**
 * EnvMapScene::Render Scene's main rendering loop.
 */

void EnvMapScene::Render()
{
	MSG msg = { 0 };

	while (true)
	{
		rndr.Render();

		if (gResized)
		{
			RECT rect;
			GetWindowRect(rndr.hWnd, &rect);
			g_settings.winW = rect.right - rect.left;
			g_settings.winH = rect.bottom - rect.top;

			rndr.UpdateViewPorts();
			gResized = false;
		}

		while (PeekMessage(&msg, rndr.hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			rndr.HandleInputs(msg);
			DispatchMessage(&msg);
		}
	}
}

/**
 * AnimationScene::LoadScene Initialize scene with lamp guy.
 */

void EnvMapScene::LoadScene()
{
    // Textures.

    g_scn.AddDiffTexture(
        "SkyPano",
        string("F:/GCRT/asset/skypano.jpg"),
        ImgLoader::LoadTexture(string("F:/GCRT/asset/skypano.jpg"))
    );

    // Lights

    DirectionalLight dirLight;
    dirLight.pos = vec3(0.0, -25.0, 25.0);
    dirLight.look = vec3(0.0, 0.0, 0.0);
    g_scn.dirLights.push_back(dirLight);

    PointLight ptLight;
    ptLight.pos = vec3(0.0, 15.0, 15.0);
    g_scn.ptLights.push_back(ptLight);

    // Materials

    RMaterial skyMat;
    skyMat.name = "SkyMat";
    skyMat.SetDiffuseTex(g_scn.diffTextures["SkyPano"].texID, "SkyPano");

    skyMat.UseShadows(false);
    skyMat.UsePhong(false);
    skyMat.UseEnvMap(false);

    g_scn.AddMaterial("SkyMat", skyMat);

    RMaterial defaultMat;
    defaultMat.name = "Default";
    defaultMat.kd = vec3(0.8, 0.8, 0.8);

    defaultMat.UseShadows(true);
    defaultMat.UsePhong(true);
    defaultMat.UseEnvMap(false);
    defaultMat.spec = 1.0;

    g_scn.AddMaterial("Default", defaultMat);

    RMaterial mirrorMat;
    mirrorMat.name = "Mirror";

    mirrorMat.UseShadows(false);
    mirrorMat.UsePhong(false);
    mirrorMat.UseEnvMap(true);

    g_scn.AddMaterial("Mirror", mirrorMat);

    // Meshes

    Sphere sph;
    sph.Create(100, 100, true);
    sph.name = "SkySphere";
    
    g_scn.AddMesh("SkySphere", make_shared<Sphere>(sph));

    Box box;
    box.Create();
    box.name = "Box";

    g_scn.AddMesh("Box", make_shared<Box>(box));

    // Models

    Model skySphere;
    skySphere.InitModelMatrices();
    skySphere.Scale(vec3(500.0, 500.0, 500.0));
    skySphere.meshName = string("SkySphere");
    skySphere.matName = string("SkyMat");
    skySphere.pickerColor = rndr.nextPickerColor();
    
    g_scn.AddModel("SkySphere0", skySphere);

    /*Model boxModel;
    boxModel.InitModelMatrices();
    boxModel.Scale(vec3(1.0, 1.0, 1.0));
    boxModel.meshName = string("Box");
    boxModel.matName = string("Mirror");
    boxModel.pickerColor = rndr.nextPickerColor();

    g_scn.AddModel("Box0", boxModel);*/

    g_scn.LoadModelFromFile(
        "Table",
        "F:/GCRT/asset/models/table/table.obj",
        "",
        "",
        rndr.nextPickerColor(),
        false
    );

    // Camera

    g_scn.cam.Init(
        vec3(5.0, 5.0, 5.0),
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        (float)g_settings.winW / (float)g_settings.winH,
        75.0f,
        1.0f,
        1000.0f
    );
}