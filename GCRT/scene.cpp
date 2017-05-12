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
    renderPass.Init(depthPass.getDepthTex());

    LoadTextures();
    InitMaterials();
    InitModels();
    InitLights();
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
    // Matte green SSS material with bumps.

    RMaterial dirtMat;
    dirtMat.name = "Dirt";
    dirtMat.SetDiffuseTex(textures["DirtDiffuse"]);
    dirtMat.SetNormalTex(textures["DirtNormal"]);
    dirtMat.UseShadows(true);
    materials["Dirt"] = dirtMat;

    RMaterial redMat;
    redMat.name = "RedMat";
    redMat.kd = vec3(0.9, 0.4, 0.4);
    redMat.UseShadows(true);
    materials["RedMat"] = redMat;

    RMaterial greenMat;
    greenMat.name = "GreenMat";
    greenMat.kd = vec3(0.4, 0.9, 0.4);
    greenMat.UseShadows(true);
    materials["GreenMat"] = greenMat;
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
    models["Plane"].SetMaterial(materials["Dirt"]);

    Box box;
    box.Create();
    box.Scale(vec3(8.0, 1.0, 4.0));
    box.Translate(vec3(-3.0, 5.0, 1.0));

    models["Box"].pGeom = make_shared<Box>(box);
    models["Box"].SetMaterial(materials["GreenMat"]);

    Sphere sph;
    sph.Create(50, 50);
    sph.Scale(vec3(2.0, 2.0, 2.0));
    sph.Translate(vec3(5.0, 5.0, 5.0));

    models["Sphere"].pGeom = make_shared<Sphere>(sph);
    models["Sphere"].SetMaterial(materials["RedMat"]);

    /*Cylinder cyl;
    cyl.Create(50);
    cyl.Scale(vec3(1.0, 1.0, 3.0));
    cyl.Translate(vec3(7.0, -5.0, 2.0));

    models["Cylinder"].pGeom = make_shared<Cylinder>(cyl);
    models["Cylinder"].SetMaterial(materials["SSS"]);*/
}

/**
 * Render - Loop through each model in the scene and draw it.
 */

void Scene::Render(HDC hDC)
{
    static float t = 0;

    // Update camera from inputs.

    cam.Update();

    depthPass.Render(models, dirLights);
    renderPass.Render(
        models,
        cam,
        dirLights,
        ptLights
    );

    // Swap.

    SwapBuffers(hDC);
    t += 0.005f;
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