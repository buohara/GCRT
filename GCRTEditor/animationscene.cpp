#include "animationscene.h"

extern RenderSettings g_settings;
extern bool gResized;
extern Scene g_scn;

/**
 * AnimationScene::LaunchAnimationScene Create scene instance and launch it.
 * @param hInstance Process handle. Need to pass to renderer for creating GL context.
 */

void AnimationScene::LaunchAnimationScene(HINSTANCE hInstance)
{
    AnimationScene animScn;
    animScn.Init(hInstance);
    animScn.Render();
}

/**
 * AnimationScene::Init Initialize scene's renderer.
 * @param hInstance Process handle to create GL context.
 */

void AnimationScene::Init(HINSTANCE hInstance)
{
    g_settings.loadSceneFromFile    = false;
    g_settings.msaaSamples          = 4;
    g_settings.useBloom             = false;
    g_settings.useDOF               = false;
    g_settings.winH                 = 1080;
    g_settings.winW                 = 1920;
    g_settings.wireFrame            = false;

    rndr.CreateRenderWindow(
        hInstance,
        "AnimationScene"
    );

    rndr.CreateGLContext();
    rndr.Init();

    DepthPass depthPass;
    depthPass.Init();

    MainPass mainPass;
    mainPass.Init(depthPass.depthTexOut, true);

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

void AnimationScene::LoadScene()
{
    // Textures.

    g_scn.AddDiffTexture(
        "DirtDiffuse",
        string("../../asset/dirtdiffuse.jpg"),
        ImgLoader::LoadTexture(string("../../asset/dirtdiffuse.jpg"))
    );

    g_scn.AddNormTexture(
        "DirtNormal",
        string("../../asset/dirtnormal.jpg"),
        ImgLoader::LoadTexture(string("../../asset/dirtnormal.jpg"))
    );

    // Lights

    DirectionalLight dirLight;
    dirLight.pos            = vec3(0.0, -25.0, 25.0);
    dirLight.look           = vec3(0.0, 0.0, 0.0);
    g_scn.dirLights.push_back(dirLight);

    PointLight ptLight;
    ptLight.pos             = vec3(0.0, 15.0, 15.0);
    g_scn.ptLights.push_back(ptLight);

    // Materials

    RMaterial defaultMat;
    defaultMat.name         = "Default";
    defaultMat.kd           = vec3(0.8, 0.8, 0.8);

    defaultMat.UseShadows(true);
    defaultMat.UsePhong(true);
    defaultMat.spec         = 1.0;
    
    g_scn.AddMaterial("Default", defaultMat);

    // Meshes

    Plane pln;
    pln.Create(10, 10);
    pln.name                = "Plane";
    pln.loadFromFile        = false;
    pln.filePath            = "NA";

    g_scn.AddMesh("Plane", make_shared<Plane>(pln));

    Model plane;
    plane.InitModelMatrices();
    plane.Scale(vec3(10.0, 10.0, 1.0));
    plane.meshName          = string("Plane");
    plane.matName           = string("Default");
    plane.pickerColor       = rndr.nextPickerColor();
    g_scn.AddModel("Plane0", plane);

    g_scn.LoadModelFromFile(
        "LampGuy",
        "../asset/models/boblampclean/boblampclean.md5mesh",
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
        90.0f,
        1.0f,
        100.0f
    );
}