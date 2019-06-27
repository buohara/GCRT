#include "testscene.h"

/**
 * TestOctree - Tests scene for determining octree BHV performance.
 */

void TestOctree()
{
    RTRenderSettings settings;
    settings.outputPath         = "D:/Git/GCRT/GCRT/renders/octreetest/";
    settings.frameFilePrefix    = "OctreeFrame";

    RTScene scn;
    scn.cam.imageH = settings.imageH;
    scn.cam.imageW = settings.imageW;
    scn.cam.aspect = (double)settings.imageW / (double)settings.imageH;

    RTMaterial whiteMat(LAMBERT, "WhiteMatte", { 0.9, 0.8, 0.7 }, 256, 16);
    scn.Add(whiteMat);
    
    RTMaterial redMat(LAMBERT, "RedMatte", { 0.9, 0.3, 0.3 }, 256, 16);
    scn.Add(redMat);

    RTMaterial greenMat(LAMBERT, "GreenMatte", { 0.3, 0.9, 0.3 }, 256, 16);
    scn.Add(greenMat);

    SphereLight lightSphWhite;
    lightSphWhite.Init(256, 16);

    lightSphWhite.pos           = { 5.0, 0.0, 3.0 };
    lightSphWhite.r             = 0.4;
    lightSphWhite.pwr           = { 50.0, 45.0, 45.0 };
    scn.lights["WhiteSphere"]   = make_shared<SphereLight>(lightSphWhite);

    RTMesh dragon(PLY, scn.GetMaterialIndex("WhiteMatte"), "../asset/models/dragon/dragon_vrip.ply");
    scn.Add(dragon);

    RTMesh cornell(CORNELL, scn.GetMaterialIndex("RedMatte"), scn.GetMaterialIndex("GreenMatte"), scn.GetMaterialIndex("WhiteMatte"));
    scn.Add(cornell);

    RTRenderer rndr(settings);
    rndr.Init();
    rndr.Render(scn);
}