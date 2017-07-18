#include "rtrenderer.h"

int CALLBACK WinMain(
    __in HINSTANCE hInstance,
    __in HINSTANCE hPrevInstance,
    __in LPSTR lpCmdLine,
    __in int nShowCmd
)
{
    uint32_t w;
    uint32_t h;

#ifdef _DEBUG
    w = 256;
    h = 256;
#else
    w = 3840;
    h = 2160;
#endif

    RTRenderer rndr;
    rndr.Init(w, h);
    rndr.Render();
    rndr.SaveImage("C:/Users/beno.NVIDIA.COM/Desktop/test.jpg");
}