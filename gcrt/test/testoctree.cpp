#include "testscene.h"

/**
 * TestOctree - Tests scene for determining octree BHV performance.
 */

void TestOctree()
{
    RTRenderSettings settings;
    settings.camType = ORTHO;

    RTRenderer rndr;
    rndr.Init();
    rndr.Render();
    
    LambertMaterial whiteMat;
    whiteMat.name   = "WhiteMatte";
    whiteMat.kd     = { 0.9, 0.8, 0.7 };
    whiteMat.Init(256, 16);

    RTMesh dragon(PLY, WHITE_MATTE, "../asset/models/dragon/dragon_vrip.ply");

    rndr.SaveImage("../../gcrt/renders/octreetest.jpg");
}