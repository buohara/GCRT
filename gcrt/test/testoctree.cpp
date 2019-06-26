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

    RTMaterial whiteMat(LAMBERT, "WhiteMatte", { 0.9, 0.8, 0.7 }, 256, 16);
    scn.Add(whiteMat);
    
    SphereLight lightSphWhite;
    lightSphWhite.Init(256, 16);

    lightSphWhite.pos           = { 0.0, 0.0, 2.0 };
    lightSphWhite.r             = 0.4;
    lightSphWhite.pwr           = { 50.0, 48.0, 42.0 };
    scn.lights["WhiteSphere"]   = make_shared<SphereLight>(lightSphWhite);

    RTMesh dragon(PLY, scn.GetMaterialIndex("WhiteMatte"), "../asset/models/dragon/dragon_vrip.ply");
    scn.Add(dragon);

    RTRenderer rndr(settings);
    rndr.Init();
    rndr.Render(scn);
}