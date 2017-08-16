#include "rtscene.h"

/**
 * [RTScene::Intersect description]
 * @param  ray [description]
 */

void RTScene::Intersect(Ray ray, Intersection &intsc)
{
    intsc.t = -1.0;
    double minDist = DBL_MAX;

    Intersection intsc2;

    for (uint32_t i = 0; i < lights.size(); i++)
    {
        lights[i].Intersect(ray, intsc2);

        if ((intsc2.t < minDist) && (intsc2.t > 0.001))
        {
            intsc = intsc2;
            minDist = intsc2.t;
        }
    }

    for (auto pMesh : meshes)
    {
        pMesh.second->Intersect(ray, intsc2);
        if ((intsc2.t < minDist) && (intsc2.t > 0.001))
        {
            intsc = intsc2;
            minDist = intsc2.t;
        }
    }
}

/**
* [RTCamera::Init description]
* @param w     [description]
* @param h     [description]
* @param posIn [description]
* @param look  [description]
* @param fov   [description]
*/

void RTCamera::Init(
    uint32_t w,
    uint32_t h,
    dvec3 posIn,
    dvec3 look,
    double fov,
    double apertureIn,
    double focalDistIn,
    uint32_t dofSamplesIn
)
{
    imageW = (double)w;
    imageH = (double)h;
    aspect = (double)w / (double)h;
    tanfov = tan(fov * pi<double>() / (360.0));
    aperture = apertureIn;
    focalDist = focalDistIn;
    dofSamples = dofSamplesIn;

    pos = posIn;
    lookDir = look - posIn;

    viewInv = lookAt(pos, look, dvec3(0.0, 0.0, 1.0));
    viewInv = inverse(viewInv);
}

/**
* [RTCamera::GenerateRay description]
* @param  pixel [description]
* @return       [description]
*/

Ray RTCamera::GeneratePrimaryRay(dvec2 pixel)
{
    Ray ray;

    dvec3 dir;
    dir.x = (2.0 * pixel.x / imageW - 1.0) * tanfov * aspect;
    dir.y = (2.0 * pixel.y / imageH - 1.0) * tanfov;
    dir.z = -1.0;
    dir = normalize(dir);

    vec4 dir4 = vec4(dir, 1);
    dir4 = viewInv * dir4;

    dir.x = dir4.x;
    dir.y = dir4.y;
    dir.z = dir4.z;

    ray.dir = dir - pos;
    ray.org = pos;

    return ray;
}

/**
 * [RTCamera::GenerateSecondaryRay description]
 * @param  primRay [description]
 * @param  pixel [description]
 * @return       [description]
 */

Ray RTCamera::GenerateSecondaryRay(Ray primRay, dvec2 pixel)
{
    Ray newRay;

    dvec3 dir;
    dir.x = (2.0 * pixel.x / imageW - 1.0) * tanfov * aspect;
    dir.y = (2.0 * pixel.y / imageH - 1.0) * tanfov;
    dir.z = -1.0;
    dir = normalize(dir);

    double t = -focalDist / dir.z;

    dvec3 intsc = primRay.org + t * primRay.dir;

    dvec3 e1 = normalize(dvec3(lookDir.z, 0.0, -lookDir.x));
    dvec3 e2 = normalize(cross(lookDir, e1));

    double theta = 2.0 * pi<double>() * (double)rand() / (double)RAND_MAX;
    double r = aperture * (double)rand() / (double)RAND_MAX;
    
    newRay.org = primRay.org;
    newRay.org += r * cos(theta) * e1;
    newRay.org += r * sin(theta) * e2;
    newRay.dir = normalize(intsc - newRay.org);

    return newRay;
}

/**
 * [RTScene::Init description]
 */

void RTScene::Init()
{
    InitMaterials();
    InitModels();
    vLights.resize(1024);
}

/**
 * [RTScene::InitMaterials description]
 */

void RTScene::InitMaterials()
{
    MirrorMaterial mirrorMat;
    mirrorMat.name = "Mirror";

    GlassMaterial glassMat;
    glassMat.name = "Glass";
    glassMat.etai = 1.0;
    glassMat.etat = 1.5;

    MatteMaterial greenMat;
    greenMat.name = "GreenMatte";
    greenMat.kd = dvec3(0.1, 0.7, 0.2);

    MatteMaterial whiteMat;
    whiteMat.name = "WhiteMatte";
    whiteMat.kd = dvec3(1.0, 0.9, 0.8);

    MatteMaterial redMat;
    redMat.name = "RedMatte";
    redMat.kd = vec3(0.7, 0.1, 0.2);

    LightMaterial lightMatBlue;
    lightMatBlue.name = "LightBlue";
    lightMatBlue.lightColor = dvec3(100.0, 200.0, 2000.0);

    LightMaterial lightMatRed;
    lightMatRed.name = "LightRed";
    lightMatRed.lightColor = dvec3(1000.0, 200.0, 100.0);

    LightMaterial lightMatWhite;
    lightMatWhite.name = "LightWhite";
    lightMatWhite.lightColor = dvec3(400.0, 360.0, 360.0);

    mats["Mirror"]      = make_shared<MirrorMaterial>(mirrorMat);
    mats["Glass"]       = make_shared<GlassMaterial>(glassMat);
    mats["GreenMatte"]  = make_shared<MatteMaterial>(greenMat);
    mats["RedMatte"]    = make_shared<MatteMaterial>(redMat);
    mats["WhiteMatte"]  = make_shared<MatteMaterial>(whiteMat);
    mats["LightBlue"]   = make_shared<LightMaterial>(lightMatBlue);
    mats["LightRed"]    = make_shared<LightMaterial>(lightMatRed);
    mats["LightWhite"]  = make_shared<LightMaterial>(lightMatWhite);
}

/**
 * [RTScene::InitModels description]
 */

void RTScene::InitModels()
{
    RTSphere redSph;
    redSph.orgn = dvec3(-10.0, 8.0, 2.0);
    redSph.r = 1.0;
    redSph.mat = "RedMatte";

    RTSphere mirrSph;
    mirrSph.orgn = dvec3(-3.0, 0.0, 2.0);
    mirrSph.r = 1.0;
    mirrSph.mat = "Glass";
    //meshes["MirrorSphere"] = make_shared<RTSphere>(mirrSph);

    RTSphere glassSph;
    glassSph.orgn = dvec3(-2.0, 0.0, 2.5);
    glassSph.r = 2.0;
    glassSph.mat = "Glass";

    RTSphere lightSphBlue;
    lightSphBlue.orgn = dvec3(0.0, -2.0, 20.0);
    lightSphBlue.r = 3.0;
    lightSphBlue.mat = "LightBlue";

    RTSphere lightSphRed;
    lightSphRed.orgn = dvec3(-2.0, 2.0, 15.0);
    lightSphRed.r = 1.0;
    lightSphRed.mat = "LightRed";

    RTSphere lightSphWhite;
    lightSphWhite.orgn = dvec3(-2.0, 2.0, 2.5);
    lightSphWhite.r = 0.4;
    lightSphWhite.mat = "LightWhite";
    lights.push_back(lightSphWhite);

    auto pMesh = make_shared<AssimpMesh>();
    pMesh->LoadModel("../asset/models/boblampclean/boblampclean.md5mesh");
    meshes["LampGuy"] = pMesh;

    auto pCornellBox = make_shared<CornellBox>();
    pCornellBox->Create();
    meshes["CornellBox"] = pCornellBox;
}

/**
 * [RTScene::LoadScene description]
 * @param filePath [description]
 */

void RTScene::LoadScene(string filePath)
{
    ifstream fin;
    fin.open(filePath);

    string line;
    istringstream iss;
    getline(fin, line);

    json scnJSON = json::parse(line.c_str());
    
    for (auto model : scnJSON["models"])
    {
        string modelName           = model["name"];
        models[modelName].mesh     = model["mesh"];
        models[modelName].material = model["material"];
    }

    for (auto mesh : scnJSON["meshes"])
    {
        string meshName = mesh["name"];
        string type = mesh["type"];

        if (type == "Sphere")
        {
            auto pSphere = make_shared<RTSphere>();
            pSphere->orgn = dvec3(0.0, 0.0, 0.0);
            pSphere->r = 1.0;
            meshes[meshName] = pSphere;
        }
        else if (type == "Box")
        {
            auto pBox = make_shared<RTBox>();
            pBox->min = dvec3(-1.0, -1.0, -1.0);
            pBox->max = dvec3(1.0, 1.0, 1.0);;
            meshes[meshName] = pBox;
        }
        else if (type == "JSON")
        {
            auto pMesh = make_shared<JSONMesh>();
            meshes[meshName] = pMesh;
        }
    }

    for (auto mat : scnJSON["materials"])
    {
        
    }

    OutputDebugString(scnJSON.dump(4).c_str());
    __debugbreak();
}

/**
 * [RTScene::GenerateLightPath description]
 * @param ray        [description]
 * @param maxDepth   [description]
 * @param depth      [description]
 * @param lightColor [description]
 */

void RTScene::GenerateLightPath(
    uint32_t setIdx,
    Ray ray,
    uint32_t maxDepth,
    uint32_t depth,
    dvec3 lightColor
)
{
    double bias = 0.001;
    Intersection intsc;

    ray.org += bias * ray.dir;
    Intersect(ray, intsc);
    ray.org -= bias * ray.dir;

    if (intsc.t > bias)
    {
        shared_ptr<RTMaterial> pMat = mats[intsc.mat];
        
        double refl     = pMat->GetReflectance(ray, intsc);
        double trans    = pMat->GetTransmittance(ray, intsc);
        double diff     = pMat->GetDiffuse(ray, intsc);

        double theta = dot(-ray.dir, intsc.normal);
        VirtualLight vLight;

        vLight.color    = theta * lightColor;
        vLight.material = intsc.mat;
        vLight.normal   = intsc.normal;
        vLight.pos      = ray.org + intsc.t * ray.dir;

        if (diff > 0.0)
        {
            dvec3 diffClr = pMat->GetDiffuseColor();
            vLight.color *= diff * diffClr;
        }

        if ((refl > 0.0) && (depth < maxDepth))
        {
            vLight.color *= refl;
            Ray newRay;
            pMat->GetReflectedRay(ray, intsc, newRay);
            GenerateLightPath(setIdx, newRay, maxDepth, depth + 1, refl * lightColor);
        }

        if ((trans > 0.0) && (depth < maxDepth))
        {
            vLight.color *= trans;
            Ray newRay;
            pMat->GetTransmittedRay(ray, intsc, newRay);
            GenerateLightPath(setIdx, newRay, maxDepth, depth + 1, trans * lightColor);
        }

        vLights[setIdx].push_back(vLight);
    }
}