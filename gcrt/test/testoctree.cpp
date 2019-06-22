#include "testscene.h"

/**
 * TestOctree - Tests scene for determining octree BHV performance.
 */

void TestOctree()
{
    RTRenderSettings settings;
    settings.camType            = ORTHO;
    settings.outputPath         = "../../gcrt/renders/octreetest/";
    settings.frameFilePrefix    = "OctreeFrame";

    RTScene scn;

    LambertMaterial whiteMat;
    whiteMat.name   = "WhiteMatte";
    whiteMat.kd     = { 0.9, 0.8, 0.7 };
    whiteMat.Init(256, 16);

    scn.Add(whiteMat);

    RTMesh dragon(PLY, scn.GetMaterialIndex("WhiteMatte"), "../asset/models/dragon/dragon_vrip.ply");
    scn.Add(dragon);

    RTRenderer rndr;
    rndr.Init();
    rndr.Render(scn);
}