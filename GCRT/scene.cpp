#include "scene.h"

/**
 * Init - Initialize scene assets.
 */

void Scene::Init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
    glClearDepth(1.0f);

    cam.Init(
        vec3(15.0, 15.0, 15.0),
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        4.0f / 3.0f,
        45.0f,
        0.1f,
        100.f
    );
    
    depthPass.Init();
    renderPass.Init();

    LoadShaders();
    LoadTextures();
    InitMaterials();
    InitModels();
    InitLights();
}

/**
 * LoadShaders - Compile shaders and cache their handles for later use.
 */

void Scene::LoadShaders()
{
    shaders["Basic"].Create(
        string("BasicShader"),
        string("BasicShader.vs"),
        string("BasicShader.fs")
    );

    shaders["BasicShadow"].Create(
        string("BasicShadow"),
        string("BasicShadow.vs"),
        string("BasicShadow.fs")
    );

    shaders["Bump"].Create(
        string("Bump"),
        string("BumpShader.vs"),
        string("BumpShader.fs")
    );

    shaders["BumpShadow"].Create(
        string("BumpShadow"),
        string("BumpShadowShader.vs"),
        string("BumpShadowShader.fs")
    );

    shaders["SSS"].Create(
        string("SSS"),
        string("SSSShader.vs"),
        string("SSSShader.fs")
    );
}

/**
 * LoadTextures - Read texture images from file and create GL textures.
 */

void Scene::LoadTextures()
{
    textures["DirtDiffuse"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/dirtdiffuse.jpg"));

    textures["DirtNormal"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/dirtnormal.jpg"));

    textures["GrassDiffuse"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/grassdiffuse.jpg"));

    textures["GrassNormal"] =
        ImgLoader::LoadTexture(string("E:/drive/GCRT/asset/grassNormal.jpg"));

    textures["DepthTex"] = depthPass.getDepthTex();
}

/**
 * InitLights - Initialize scene lights.
 */

void Scene::InitLights()
{
    DirectionalLight dirLight;
    dirLight.pos = vec3(0.0, 15.0, 15.0);
    dirLight.look = vec3(0.0, 0.0, 0.0);
    dirLights.push_back(dirLight);

    PointLight ptLight;
    ptLight.pos = vec3(0.0, 15.0, 15.0);
    ptLights.push_back(ptLight);
}

/**
 * InitMaterials - Build materials from shaders and textures.
 */

void Scene::InitMaterials()
{
    // Dirt material with normal map.

    BumpMaterial dirtMat;
    dirtMat.name = "Dirt";
    dirtMat.diffuseTexID = textures["DirtDiffuse"];
    dirtMat.normalTexID = textures["DirtNormal"];
    dirtMat.program = shaders["Bump"].program;
    materials["Dirt"] = make_shared<BumpMaterial>(dirtMat);

    // Grass material with normal map.

    BumpMaterial grassMat;
    dirtMat.name = "Grass";
    dirtMat.diffuseTexID = textures["GrassDiffuse"];
    dirtMat.normalTexID = textures["GrassNormal"];
    dirtMat.program = shaders["Bump"].program;
    materials["Grass"] = make_shared<BumpMaterial>(dirtMat);

    // Matte blue material.

    BasicMaterial basicBlueMat;
    basicBlueMat.name = "BasicBlue";
    basicBlueMat.program = shaders["Basic"].program;
    basicBlueMat.kd = vec3(0.1, 0.1, 0.7);
    materials["BasicBlue"] = make_shared<BasicMaterial>(basicBlueMat);

    // Matte green material with shadows.

    BasicShadowMaterial basicShadowMat;
    basicShadowMat.name = "BasicShadow";
    basicShadowMat.program = shaders["BasicShadow"].program;
    basicShadowMat.kd = vec3(0.1, 0.7, 0.1);
    basicShadowMat.depthTexID = textures["DepthTex"];
    materials["BasicShadow"] = make_shared<BasicShadowMaterial>(basicShadowMat);

    // Matte green material with shadows.

    SSSMaterial sssMat;
    sssMat.name = "SSS";
    sssMat.program = shaders["SSS"].program;
    sssMat.kd = vec3(0.9, 0.9, 0.9);
    sssMat.depthTexID = textures["DepthTex"];
    materials["SSS"] = make_shared<SSSMaterial>(sssMat);

    // Dirt material with normal and shadow mapping.

    BumpShadowMaterial bumpShadowDirtMat;
    bumpShadowDirtMat.name = "BumpShadowDirt";
    bumpShadowDirtMat.program = shaders["BumpShadow"].program;
    bumpShadowDirtMat.diffuseTexID = textures["DirtDiffuse"];
    bumpShadowDirtMat.normalTexID = textures["DirtNormal"];
    bumpShadowDirtMat.depthTexID = textures["DepthTex"];
    materials["BumpShadowDirt"] = make_shared<BumpShadowMaterial>(bumpShadowDirtMat);

    // Grass material with normal and shadow mapping.

    BumpShadowMaterial bumpShadowGrassMat;
    bumpShadowGrassMat.name = "BumpShadowGrass";
    bumpShadowGrassMat.program = shaders["BumpShadow"].program;
    bumpShadowGrassMat.diffuseTexID = textures["GrassDiffuse"];
    bumpShadowGrassMat.normalTexID = textures["GrassNormal"];
    bumpShadowGrassMat.depthTexID = textures["DepthTex"];
    materials["BumpShadowGrass"] = make_shared<BumpShadowMaterial>(bumpShadowGrassMat);
}

/**
 * Create models from geometries and materials.
 */

void Scene::InitModels()
{
    // Plane

    Plane pln;
    pln.Create(10, 10);
    pln.Scale(vec3(10.0, 10.0, 1.0));

    models["Plane"].pGeom = make_shared<Plane>(pln);
    models["Plane"].SetMaterial(materials["BumpShadowGrass"]);

    Box box;
    box.Create();
    box.Scale(vec3(8.0, 1.0, 4.0));
    box.Translate(vec3(-3.0, 5.0, 1.0));

    models["Box"].pGeom = make_shared<Box>(box);
    models["Box"].SetMaterial(materials["SSS"]);

    Sphere sph;
    sph.Create(50, 50);
    sph.Scale(vec3(2.0, 1.0, 2.0));
    sph.Translate(vec3(5.0, 5.0, 5.0));

    models["Sphere"].pGeom = make_shared<Sphere>(sph);
    models["Sphere"].SetMaterial(materials["SSS"]);

    Cylinder cyl;
    cyl.Create(50);
    cyl.Scale(vec3(2.0, 2.0, 5.0));
    cyl.Translate(vec3(7.0, -5.0, 2.0));

    models["Cylinder"].pGeom = make_shared<Cylinder>(cyl);
    models["Cylinder"].SetMaterial(materials["SSS"]);
}

/**
 * Render - Loop through each model in the scene and draw it.
 */

void Scene::Render(HDC hDC)
{
    static float t = 0;

    // Update camera from inputs.

    cam.Update();

    // Do depth pass.

    depthPass.Render(models, dirLights);

    // Camera render pass.

    renderPass.Render(
        models, 
        dirLights, 
        ptLights, 
        cam
    );

    // Swap.

    SwapBuffers(hDC);
    t += 0.01f;
}

/**
 * HandleInputs - Process any keyboard and mouse inputs.
 *
 * @param msg Windows event message to handle.
 */

void Scene::HandleInputs(MSG &msg)
{
    switch (msg.message)
    {
    case WM_KEYDOWN:

        if (msg.wParam == 0x46)
        {
            if (wireFrame == false)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                wireFrame = true;
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                wireFrame = false;
            }
        }

        cam.HandleKeyDown(msg.wParam);
        break;

    case WM_KEYUP:

        cam.HandleKeyUp(msg.wParam);
        break;

    case WM_MOUSEMOVE:

        cam.HandleMouseMove(msg.lParam);
        break;

    case WM_LBUTTONDOWN:

        cam.HandleMouseDown(msg.lParam);
        break;

    case WM_LBUTTONUP:

        cam.HandleMouseUp();
        break;

    default:
        break;
    }
}