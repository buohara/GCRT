#include "scene.h"

/**
 * Create a scene.
 */

Scene::Scene()
{
}

/**
 * AddDiffTexture - 
 */

void Scene::AddDiffTexture(string name, string path, GLuint id)
{
    diffTextures[name].texID = id;
    diffTextures[name].imagePath = path;
    diffTexNames.resize(0);

    map<string, Tex>::iterator it;

    for (it = diffTextures.begin(); it != diffTextures.end(); it++)
    {
        diffTexNames.push_back((*it).first.c_str());
    }
}

/**
 * AddNormTexture
 */

void Scene::AddNormTexture(string name, string path, GLuint id)
{
    normTextures[name].texID = id;
    normTextures[name].imagePath = path;
    normTexNames.resize(0);

    map<string, Tex>::iterator it;

    for (it = normTextures.begin(); it != normTextures.end(); it++)
    {
        normTexNames.push_back((*it).first.c_str());
    }
}

/**
 * Save scene to file.
 */

void Scene::Save(string file)
{
    ofstream fout;
    //fout.open(file.c_str(), 'w');
    fout.open("C:/Users/SLI/Desktop/scene.scn", 'w');

    // Textures

    map<string, Tex>::iterator texIt;

    fout << "DIFFUSE TEXTURES" << diffTextures.size() << endl;

    for (texIt = diffTextures.begin(); texIt != diffTextures.end(); texIt++)
    {
        if ((*texIt).second.imagePath == "")
        {
            continue;
        }

        fout << (*texIt).first << endl;
        fout << (*texIt).second.imagePath << endl;
    }

    fout << "NORMAL TEXTURES" << normTextures.size() << endl;

    for (texIt = normTextures.begin(); texIt != normTextures.end(); texIt++)
    {
        if ((*texIt).second.imagePath == "")
        {
            continue;
        }

        fout << (*texIt).first << endl;
        fout << (*texIt).second.imagePath << endl;
    }

    map<string, RMaterial>::iterator matIt;

    fout << "MATERIALS" << materials.size() << endl;

    for (matIt = materials.begin(); matIt != materials.end(); matIt++)
    {
        fout << (*matIt).first << endl;
        fout << (*matIt).second.useNormalMap << endl;
        fout << (*matIt).second.useDiffuseMap << endl;
        fout << (*matIt).second.useSSS << endl;
        fout << (*matIt).second.useShadows << endl;
        fout << (*matIt).second.diffTexName << endl;
        fout << (*matIt).second.normalTexName << endl;
        fout << (*matIt).second.kd.x << (*matIt).second.kd.y << (*matIt).second.kd.z << endl;
        fout << (*matIt).second.spec << endl;
    }

    map<string, Model>::iterator modIt;

    fout << "MODELS" << models.size() << endl;

    for (modIt = models.begin(); modIt != models.end(); modIt++)
    {
        fout << (*modIt).first << endl;
        fout << (*modIt).second.meshName;
        fout << (*modIt).second.matName << endl;
        fout << (*modIt).second.pos.x << (*modIt).second.pos.y << (*modIt).second.pos.z << endl;
        fout << (*modIt).second.dims.x << (*modIt).second.dims.y << (*modIt).second.dims.z << endl;
        
        fout 
            << (*modIt).second.pickerColor.x 
            << (*modIt).second.pickerColor.y 
            << (*modIt).second.pickerColor.z 
            << endl;
    }

    fout << "CAMERA" << endl;

    fout << cam.pos.x << cam.pos.y << cam.pos.z << endl;
    fout << cam.lookDir.x << cam.lookDir.y << cam.lookDir.z << endl;
    fout << cam.aspect << endl;
    fout << cam.fov << endl;
    fout << cam.nclip << endl;
    fout << cam.fclip << endl;

    fout << "DIRECTIONAL LIGHTS" << dirLights.size() << endl;

    for (int i  = 0; i < dirLights.size(); i++)
    {
        fout << dirLights[i].pos.x << dirLights[i].pos.y << dirLights[i].pos.z << endl;
        fout << dirLights[i].look.x << dirLights[i].look.y << dirLights[i].look.z << endl;
    }

    fout << "POINT LIGHTS" << ptLights.size() << endl;

    for (int i = 0; i < ptLights.size(); i++)
    {
        fout << ptLights[i].pos.x << ptLights[i].pos.y << ptLights[i].pos.z << endl;
    }

    fout.close();
}

/**
 * Load scene from file.
 */

void Scene::Load(string file)
{
    ifstream fin;
    fin.open("C:/Users/SLI/Desktop/scene.scn");
}

/**
 * Add Material
 */

void Scene::AddMaterial(string name, RMaterial mat)
{
    materials[name] = mat;
    materialNames.resize(0);

    map<string, RMaterial>::iterator it;

    for (it = materials.begin(); it != materials.end(); it++)
    {
        materialNames.push_back((*it).first.c_str());
    }
}

/**
 * Add Mesh
 */

void Scene::AddMesh(string name, shared_ptr<Mesh> pGeom)
{
    meshes[name] = pGeom;

    meshNames.resize(0);

    map<string, shared_ptr<Mesh>>::iterator it;

    for (it = meshes.begin(); it != meshes.end(); it++)
    {
        meshNames.push_back((*it).first.c_str());
    }
}

/**
 * Add model
 */

void Scene::AddModel(string name, Model model)
{
    models[name] = model;
    modelNames.resize(0);

    map<string, Model>::iterator it;

    for (it = models.begin(); it != models.end(); it++)
    {
        modelNames.push_back((*it).first.c_str());
    }
}

/**
 * LoadBlenderModel
 */

void Scene::LoadBlenderModel(
    string name,
    string blendFile,
    string diffuseTexFile,
    string normalTexFile,
    vec3 pickerColor
)
{
    BlenderMesh mesh;
    mesh.Create(blendFile);
    meshes[name] = make_shared<BlenderMesh>(mesh);

    Model model;
    model.meshName = name;
    model.matName = string("Default");
    model.pickerColor = pickerColor;
    model.InitModelMatrices();
    model.Scale(vec3(0.25, 0.25, 0.25));
    model.Translate(vec3(-2.0, -2.0, 5.0));
    model.Rotate(pi<float>(), vec3(0.0, 1.0, 0.0));

    AddModel(name, model);
}