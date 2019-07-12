#include "envmapscene.h"

extern RenderSettings g_settings;
extern bool gResized;

/**
 * EnvMapScene::LaunchEnvMapScene Create scene instance and launch it.
 * @param hInstance Process handle. Need to pass to renderer for creating GL context.
 */

void EnvMapScene::LaunchEnvMapScene(HINSTANCE hInstance)
{
    EnvMapScene envScn(hInstance);
    envScn.Render();
}

/**
 * EnvMapScene::Init Initialize scene's renderer.
 * @param hInstance Process handle to create GL context.
 */

EnvMapScene::EnvMapScene(HINSTANCE hInstance) : scn(1920, 1080),
    rndr(hInstance, "EnvMapScene")
{
    g_settings.loadSceneFromFile    = false;
    g_settings.msaaSamples          = 4;
    g_settings.useBloom             = false;
    g_settings.useDOF               = false;
    g_settings.winH                 = 1080;
    g_settings.winW                 = 1920;
    g_settings.wireFrame            = false;
    g_settings.useSkyBox            = true;

    rndr.Init(scn);
    rndr.CreateRenderWindow(hInstance, "AnimationScene");
    rndr.CreateGLContext();

    scn.SetSkyTex(string("F:/GCRT/asset/skypano.jpg"),
        ImgLoader::LoadTexture(string("F:/GCRT/asset/skypano.jpg")));

    MainPass mainPass;
    mainPass.Init(0, true, scn);

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
        rndr.Render(scn);

        if (gResized)
        {
            RECT rect;
            GetWindowRect(rndr.hWnd, &rect);
            g_settings.winW = rect.right - rect.left;
            g_settings.winH = rect.bottom - rect.top;

            rndr.UpdateViewPorts(scn);
            gResized = false;
        }

        while (PeekMessage(&msg, rndr.hWnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            rndr.HandleInputs(msg, scn);
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

    scn.AddDiffTexture(
        "SkyPano",
        string("F:/GCRT/asset/skypano.jpg"),
        ImgLoader::LoadTexture(string("F:/GCRT/asset/skypano.jpg"))
    );

    // Lights

    Light dirLight(DIRECTIONAL, vec3(10.0, 10.0, 10.0));
    scn.lights.push_back(dirLight);

    Light ptLight(POINTLIGHT, vec3(0.0, 15.0, 15.0));
    ptLight.pos = vec3(0.0, 15.0, 15.0);
    scn.lights.push_back(ptLight);

    // Materials

    RMaterial skyMat;
    skyMat.name = "SkyMat";
    skyMat.SetDiffuseTex(scn.diffTextures["SkyPano"].texID, "SkyPano");

    skyMat.UseShadows(false);
    skyMat.UsePhong(false);
    skyMat.UseEnvMap(false);

    scn.AddMaterial("SkyMat", skyMat);

    RMaterial defaultMat;
    defaultMat.name = "Default";
    defaultMat.kd = vec3(0.8, 0.8, 0.8);

    defaultMat.UseShadows(true);
    defaultMat.UsePhong(true);
    defaultMat.UseEnvMap(false);
    defaultMat.spec = 1.0;

    scn.AddMaterial("Default", defaultMat);

    RMaterial mirrorMat;
    mirrorMat.name = "Mirror";

    mirrorMat.UseShadows(false);
    mirrorMat.UsePhong(false);
    mirrorMat.UseEnvMap(true);

    scn.AddMaterial("Mirror", mirrorMat);

    // Meshes

    MeshGL sph(SPHERE, 100, 100, true);
    sph.name = "SkySphere";
    sph.Scale(vec3(500.0, 500.0, 500.0));
    scn.AddMesh("SkySphere", sph);
    sph.pickerColor = rndr.nextPickerColor();

    MeshGL box(BOX);
    box.name = "Box";
    scn.AddMesh("Box", box);

    MeshGL tbl(SKELETAL, "F:/GCRT/asset/models/table/table.obj");
    tbl.name = "Table";
    tbl.pickerColor = rndr.nextPickerColor();
    tbl.invert = false;
    scn.AddMesh("Table", tbl);
}