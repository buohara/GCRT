#include "animationscene.h"

extern RenderSettings g_settings;
extern bool gResized;

/**
 * AnimationScene::LaunchAnimationScene Create scene instance and launch it.
 * @param hInstance Process handle. Need to pass to renderer for creating GL context.
 */

void AnimationScene::LaunchAnimationScene(HINSTANCE hInstance)
{
    AnimationScene animScn(hInstance);
    animScn.Render();
}

/**
 * AnimationScene::Init Initialize scene's renderer.
 * @param hInstance Process handle to create GL context.
 */

AnimationScene::AnimationScene(HINSTANCE hInstance) : scn(1920, 1080),
    rndr(hInstance, "AnimationScene")
{
    g_settings.loadSceneFromFile    = false;
    g_settings.msaaSamples          = 4;
    g_settings.useBloom             = false;
    g_settings.useDOF               = false;
    g_settings.wireFrame            = false;

    rndr.Init(scn);
    rndr.CreateRenderWindow(hInstance, "AnimationScene");
    rndr.CreateGLContext();

    DepthPass depthPass;
    depthPass.Init();

    MainPass mainPass;
    mainPass.Init(depthPass.depthTexOut, true, scn);

    rndr.passes["DepthPass"]    = make_shared<DepthPass>(depthPass);
    rndr.passes["MainPass"]     = make_shared<MainPass>(mainPass);

    LoadScene();
}

/**
 * AnimationScene::Render Scene's main rendering loop.
 */

void AnimationScene::Render()
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

void AnimationScene::LoadScene()
{
    // Textures.

    scn.AddDiffTexture(
        "DirtDiffuse",
        string("../../asset/dirtdiffuse.jpg"),
        ImgLoader::LoadTexture(string("../../asset/dirtdiffuse.jpg"))
    );

    scn.AddNormTexture(
        "DirtNormal",
        string("../../asset/dirtnormal.jpg"),
        ImgLoader::LoadTexture(string("../../asset/dirtnormal.jpg"))
    );

    // Lights

    Light dirLight(DIRECTIONAL, vec3(0.0, -25.0, 25.0));
    scn.lights.push_back(dirLight);

    Light ptLight(POINTLIGHT, vec3(0.0, 15.0, 15.0));
    scn.lights.push_back(ptLight);

    // Materials

    RMaterial defaultMat;
    defaultMat.name         = "Default";
    defaultMat.kd           = vec3(0.8, 0.8, 0.8);

    defaultMat.UseShadows(true);
    defaultMat.UsePhong(true);
    defaultMat.spec         = 1.0;
    
    scn.AddMaterial("Default", defaultMat);

    // Meshes

    MeshGL pln(PLANE, 10, 10);

    pln.name                = "Plane";
    pln.loadFromFile        = false;
    pln.filePath            = "NA";
    pln.Scale(vec3(10.0, 10.0, 1.0));

    scn.AddMesh("Plane", pln);

    pln.matName           = string("Default");
    pln.pickerColor       = rndr.nextPickerColor();

    MeshGL lampGuy(SKELETAL, "../asset/models/boblampclean/boblampclean.md5mesh");
}