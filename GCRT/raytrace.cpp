#include "rtrenderer.h"

int CALLBACK WinMain(
    __in HINSTANCE hInstance,
    __in HINSTANCE hPrevInstance,
    __in LPSTR lpCmdLine,
    __in int nShowCmd
)
{
    RTRenderer rndr;
    rndr.Init();
    rndr.Render();
    rndr.SaveImage("C:/Users/beno.NVIDIA.COM/Desktop/test.jpg");
}

int main(int argc, char** argv)
{
    RTRenderer rndr;
    rndr.Init();
    rndr.Render();
    rndr.SaveImage("C:/Users/beno.NVIDIA.COM/Desktop/test.jpg");
}