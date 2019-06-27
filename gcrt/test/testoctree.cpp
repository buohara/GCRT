#include "testscene.h"

/**
 * TestOctree - Tests scene for determining octree BHV performance.
 */

void TestOctree()
{
    RTRenderSettings settings;
    settings.outputPath         = "F:/GCRT/gcrt/renders/octreetest/";
    settings.frameFilePrefix    = "OctreeFrame";

    RTScene scn;
    scn.cam.imageH = settings.imageH;
    scn.cam.imageW = settings.imageW;
    scn.cam.aspect = (double)settings.imageW / (double)settings.imageH;

    RTMaterial whiteMat(LAMBERT, "WhiteMatte", { 0.9, 0.8, 0.7 }, 256, 16);
    scn.Add(whiteMat);
    
    SphereLight lightSphWhite;
    lightSphWhite.Init(256, 16);

    lightSphWhite.pos           = { 0.0, 0.0, 8.0 };
    lightSphWhite.r             = 0.4;
    lightSphWhite.pwr           = { 100.0, 90.0, 90.0 };
    scn.lights["WhiteSphere"]   = make_shared<SphereLight>(lightSphWhite);

    RTMesh dragon(PLY, scn.GetMaterialIndex("WhiteMatte"), "../asset/models/dragon/dragon_vrip.ply");
    scn.Add(dragon);

    RTRenderer rndr(settings);
    rndr.Init();
    rndr.Render(scn);
}