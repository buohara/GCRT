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

    fout << "DIFFUSE_TEXTURES " << diffTextures.size() - 1 << "\n" << endl;

    for (texIt = diffTextures.begin(); texIt != diffTextures.end(); texIt++)
    {
        if ((*texIt).second.imagePath == "")
        {
            continue;
        }

        fout << (*texIt).first << endl;
        fout << (*texIt).second.imagePath << endl;
        fout << endl;
    }

    fout << "NORMAL_TEXTURES " << normTextures.size() << "\n" << endl;

    for (texIt = normTextures.begin(); texIt != normTextures.end(); texIt++)
    {
        if ((*texIt).second.imagePath == "")
        {
            continue;
        }

        fout << (*texIt).first << endl;
        fout << (*texIt).second.imagePath << endl;
        fout << endl;
    }

    map<string, RMaterial>::iterator matIt;

    fout << "MATERIALS " << materials.size() << "\n" << endl;

    for (matIt = materials.begin(); matIt != materials.end(); matIt++)
    {
        fout << (*matIt).first << endl;
        fout << (*matIt).second.useNormalMap << endl;
        fout << (*matIt).second.useDiffuseMap << endl;
        fout << (*matIt).second.useSSS << endl;
        fout << (*matIt).second.useShadows << endl;
        fout << (*matIt).second.diffTexName << endl;
        fout << (*matIt).second.normalTexName << endl;
        
        fout << 
            (*matIt).second.kd.x << " " <<
            (*matIt).second.kd.y << " " <<
            (*matIt).second.kd.z << endl;
        
        fout << (*matIt).second.spec << endl;
        fout << endl;
    }

    map<string, shared_ptr<Mesh>>::iterator meshIt;

    fout << "MESHES " << meshes.size() << "\n" << endl;

    for (meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
    {
        fout << (*meshIt).second->name << endl;
        fout << (*meshIt).second->blenderModel << endl;
        fout << (*meshIt).second->blenderPath << endl;
        fout << endl;
    }

    map<string, Model>::iterator modIt;

    fout << "MODELS " << models.size() << "\n" << endl;

    for (modIt = models.begin(); modIt != models.end(); modIt++)
    {
        fout << (*modIt).first << endl;
        fout << (*modIt).second.meshName << endl;
        fout << (*modIt).second.matName << endl;
        
        fout << 
            (*modIt).second.pos.x << " " <<
            (*modIt).second.pos.y << " " <<
            (*modIt).second.pos.z << endl;
        
        fout << 
            (*modIt).second.dims.x << " " <<
            (*modIt).second.dims.y << " " <<
            (*modIt).second.dims.z << endl;
        
        fout 
            << (*modIt).second.pickerColor.x << " "
            << (*modIt).second.pickerColor.y << " "
            << (*modIt).second.pickerColor.z << " "
            << endl;

        fout << endl;
    }

    fout << "CAMERA " << "\n" << endl;

    fout << 
        cam.pos.x << " " <<
        cam.pos.y << " " <<
        cam.pos.z << endl;
    
    fout << 
        cam.lookDir.x << " " <<
        cam.lookDir.y << " " <<
        cam.lookDir.z << endl;
    
    fout << cam.aspect << endl;
    fout << cam.fov << endl;
    fout << cam.nclip << endl;
    fout << cam.fclip << endl;
    fout << endl;

    fout << "DIRECTIONAL_LIGHTS " << dirLights.size() << "\n" << endl;

    for (int i  = 0; i < dirLights.size(); i++)
    {
        fout <<
            dirLights[i].pos.x << " " <<
            dirLights[i].pos.y << " " <<
            dirLights[i].pos.z << endl;

        fout << 
            dirLights[i].look.x << " " << 
            dirLights[i].look.y << " " <<
            dirLights[i].look.z << endl;

        fout << endl;
    }

    fout << "POINT_LIGHTS " << ptLights.size() << "\n" << endl;

    for (int i = 0; i < ptLights.size(); i++)
    {
        fout << 
            ptLights[i].pos.x << " " <<
            ptLights[i].pos.y << " " <<
            ptLights[i].pos.z << endl;

        fout << endl;
    }

    fout.close();
}

/**
 * Load scene from file.
 */

void Scene::Load(string file)
{
    ifstream fin;
    fin.open(file);

    string line;
    istringstream iss;

    // Diffuse textures.

    uint32_t numTextures;
    string sectionName;

    getline(fin, line);
    iss.str(line);
    iss >> sectionName >> numTextures;
    iss.clear();
    getline(fin, line);

    for (uint32_t i = 0; i < numTextures; i++)
    {
        string texName;
        string texPath;

        getline(fin, line);
        iss.str(line);
        iss >> texName;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> texPath;
        iss.clear();

        getline(fin, line);

        AddDiffTexture(texName, texPath, ImgLoader::LoadTexture(texPath));
    }

    // Normal textures

    getline(fin, line);
    iss.str(line);
    iss >> sectionName >> numTextures;
    iss.clear();
    getline(fin, line);

    for (uint32_t i = 0; i < numTextures; i++)
    {
        string texName;
        string texPath;

        getline(fin, line);
        iss.str(line);
        iss >> texName;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> texPath;
        iss.clear();

        getline(fin, line);

        AddNormTexture(texName, texPath, ImgLoader::LoadTexture(texPath));
    }

    // Materials

    uint32_t numMaterials;

    getline(fin, line);
    iss.str(line);
    iss >> sectionName >> numMaterials;
    iss.clear();
    getline(fin, line);

    for (uint32_t i = 0; i < numMaterials; i++)
    {
        string name;
        bool useNormalMap;
        bool useDiffuseMap;
        bool useSSS;
        bool useShadows;
        string diffuseTexName;
        string normalTexName;
        vec3 kd;
        float spec;

        getline(fin, line);
        iss.str(line);
        iss >> name;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> useNormalMap;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> useDiffuseMap;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> useSSS;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> useShadows;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> diffuseTexName;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> normalTexName;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> kd.x >> kd.y >> kd.z;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> spec;
        iss.clear();

        getline(fin, line);

        RMaterial newMat;
        newMat.name = name;
        newMat.kd = kd;
        newMat.useShadows = useShadows;
        newMat.useDiffuseMap = useDiffuseMap;
        newMat.useNormalMap = useNormalMap;
        newMat.useSSS = useSSS;
        newMat.spec = spec;
        newMat.diffTexName = diffuseTexName;
        newMat.normalTexName = normalTexName;
        
        if (newMat.useDiffuseMap)
        {
            newMat.diffuseTexID = diffTextures[diffuseTexName].texID;
        }

        if (newMat.useNormalMap)
        {
            newMat.normalTexID = normTextures[normalTexName].texID;
        }

        AddMaterial(name, newMat);
    }

    // Meshes

    uint32_t numMeshes;

    getline(fin, line);
    iss.str(line);
    iss >> sectionName >> numMeshes;
    iss.clear();
    getline(fin, line);

    for (uint32_t i = 0; i < numMeshes; i++)
    {
        string name;
        bool blenderModel;
        string blenderPath;

        getline(fin, line);
        iss.str(line);
        iss >> name;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> blenderModel;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> blenderPath;
        iss.clear();

        getline(fin, line);

        if (name == "Sphere")
        {
            Sphere sph;
            sph.name = "Sphere";
            sph.Create(50, 50);
            sph.blenderModel = false;
            sph.blenderPath = "NA";
            AddMesh("Sphere", make_shared<Sphere>(sph));
            continue;
        }

        if (name == "Plane")
        {
            Plane pln;
            pln.Create(10, 10);
            pln.name = "Plane";
            pln.blenderModel = false;
            pln.blenderPath = "NA";
            AddMesh("Plane", make_shared<Plane>(pln));
            continue;
        }

        if (name == "Box")
        {
            Box box;
            box.Create();
            box.name = "Box";
            box.blenderModel = false;
            box.blenderPath = "NA";
            AddMesh("Box", make_shared<Box>(box));
            continue;
        }

        if (name == "Cylinder")
        {
            Cylinder cyl;
            cyl.Create(15);
            cyl.name = "Cylinder";
            cyl.blenderModel = false;
            cyl.blenderPath = "NA";
            AddMesh("Cylinder", make_shared<Cylinder>(cyl));
            continue;
        }

        if (blenderModel == true)
        {
            LoadBlenderModel(name, blenderPath, "NA", "NA", vec3(0.0, 0.0, 0.0), true);
        }
    }

    // Models

    uint32_t numModels;

    getline(fin, line);
    iss.str(line);
    iss >> sectionName >> numModels;
    iss.clear();
    getline(fin, line);

    for (uint32_t i = 0; i < numModels; i++)
    {
        string name;
        string meshName;
        string matName;
        vec3 pos;
        vec3 dims;
        vec3 pickerColor;

        getline(fin, line);
        iss.str(line);
        iss >> name;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> meshName;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> matName;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> pos.x >> pos.y >> pos.z;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> dims.x >> dims.y >> dims.z;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> pickerColor.x >> pickerColor.y >> pickerColor.z;
        iss.clear();

        getline(fin, line);

        Model newModel;
        newModel.meshName = meshName;
        newModel.matName = matName;
        newModel.pickerColor = pickerColor;
        newModel.InitModelMatrices();
        newModel.Translate(pos);
        newModel.Scale(dims);
        newModel.selected = false;

        if (meshes[meshName]->blenderModel == true)
        {
            newModel.Rotate(vec3(0.0, pi<float>(), 0.0));
        }

        AddModel(name, newModel);
    }

    // Camera

    getline(fin, line);
    iss.str(line);
    iss >> sectionName;
    iss.clear();
    getline(fin, line);

    {
        vec3 pos;
        vec3 look;
        float fov;
        float aspect;
        float nclip;
        float fclip;

        getline(fin, line);
        iss.str(line);
        iss >> pos.x >> pos.y >> pos.z;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> look.x >> look.y >> look.z;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> aspect;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> fov;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> nclip;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> fclip;
        iss.clear();

        getline(fin, line);

        cam.pos = pos;
        cam.lookDir = look;
        cam.fov = fov;
        cam.aspect = aspect;
        cam.nclip = nclip;
        cam.fclip = fclip;
        cam.up = vec3(0.0, 0.0, 1.0);
    }

    // Lights

    uint32_t numDirLights;

    getline(fin, line);
    iss.str(line);
    iss >> sectionName >> numDirLights;
    iss.clear();
    getline(fin, line);

    for (uint32_t i = 0; i < numDirLights; i++)
    {
        vec3 pos;
        vec3 dir;

        getline(fin, line);
        iss.str(line);
        iss >> pos.x >> pos.y >> pos.z;
        iss.clear();

        getline(fin, line);
        iss.str(line);
        iss >> dir.x >> dir.y >> dir.z;
        iss.clear();

        getline(fin, line);

        DirectionalLight dirLight;
        dirLight.pos = pos;
        dirLight.look = dir;

        dirLights.push_back(dirLight);
    }

    uint32_t numPtLights;

    getline(fin, line);
    iss.str(line);
    iss >> sectionName >> numPtLights;
    iss.clear();
    getline(fin, line);

    for (uint32_t i = 0; i < numPtLights; i++)
    {
        vec3 pos;

        getline(fin, line);
        iss.str(line);
        iss >> pos.x >> pos.y >> pos.z;
        iss.clear();

        getline(fin, line);

        PointLight ptLight;
        ptLight.pos = pos;

        ptLights.push_back(ptLight);
    }

    fin.close();
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
    vec3 pickerColor,
    bool meshOnly
)
{
    BlenderMesh mesh;
    mesh.Create(blendFile);
    mesh.blenderModel = true;
    mesh.blenderPath = blendFile;
    mesh.name = name;
    AddMesh(name, make_shared<BlenderMesh>(mesh));

    if (meshOnly)
    {
        return;
    }

    Model model;
    model.meshName = name;
    model.matName = string("Default");
    model.pickerColor = pickerColor;
    model.InitModelMatrices();
    model.Scale(vec3(0.25, 0.25, 0.25));
    model.Translate(vec3(-2.0, -2.0, 5.0));
    model.Rotate(vec3(0.0, pi<float>(), 0.0));

    AddModel(name, model);
}