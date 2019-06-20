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
    
    rndr.SaveImage("../../gcrt/renders/octreetest.jpg");
}