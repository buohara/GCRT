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

void Scene::AddDiffTexture(string name, GLuint id)
{
    diffTextures[name] = id;
    diffTexNames.resize(0);

    map<string, GLuint>::iterator it;

    for (it = diffTextures.begin(); it != diffTextures.end(); it++)
    {
        diffTexNames.push_back((*it).first.c_str());
    }
}

/**
 * AddNormTexture
 */

void Scene::AddNormTexture(string name, GLuint id)
{
    normTextures[name] = id;
    normTexNames.resize(0);

    map<string, GLuint>::iterator it;

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