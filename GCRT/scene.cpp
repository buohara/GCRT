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
    fout.open(file.c_str(), 'w');

    // Textures

    map<string, Tex>::iterator texIt;

    fout << "DIFFUSE TEXTURES" << endl;
    fout << diffTextures.size() << endl;

    for (texIt = diffTextures.begin(); texIt != diffTextures.end(); texIt++)
    {
        if ((*texIt).second.imagePath == "")
        {
            continue;
        }

        fout << (*texIt).first << endl;
        fout << (*texIt).second.imagePath << endl;
    }

    fout << "NORMAL TEXTURES" << endl;
    fout << normTextures.size() << endl;

    for (texIt = normTextures.begin(); texIt != normTextures.end(); texIt++)
    {
        fout << (*texIt).first << endl;
        fout << (*texIt).second.imagePath << endl;
    }

    map<string, RMaterial>::iterator matIt;

    fout << "MATERIALS" << endl;
    fout << materials.size() << endl;

    for (matIt = materials.begin(); matIt != materials.end(); matIt++)
    {
        fout << (*matIt).first << endl;
        fout << (*matIt).second.diffTexName << endl;
        fout << (*matIt).second.normalTexName << endl;
        fout << (*matIt).second.kd.x << endl;
    }

    map<string, Model>::iterator modIt;

    fout << "MODELS" << endl;
    fout << models.size() << endl;

    for (modIt = models.begin(); modIt != models.end(); modIt++)
    {
        fout << (*modIt).first << endl;
        fout << (*modIt).second.mat.name << endl;
    }

    fout << "CAMERA" << endl;

    fout << "DIRECTIONAL LIGHTS" << endl;
    fout << dirLights.size() << endl;

    for (texIt = normTextures.begin(); texIt != normTextures.end(); texIt++)
    {
    }

    fout << "POINT LIGHTS" << endl;
    fout << ptLights.size() << endl;

    for (texIt = normTextures.begin(); texIt != normTextures.end(); texIt++)
    {
    }

    fout.close();
}

/**
 * Load scene from file.
 */

void Scene::Load(string file)
{

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
    model.pMesh = meshes[name];
    model.SetMaterial(materials["Default"]);
    model.pickerColor = pickerColor;
    model.InitModelMatrices();
    model.Scale(vec3(0.25, 0.25, 0.25));
    model.Translate(vec3(-2.0, -2.0, 5.0));
    model.Rotate(pi<float>(), vec3(0.0, 1.0, 0.0));

    AddModel(name, model);
}