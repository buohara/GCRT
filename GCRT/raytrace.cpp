#include "rtrenderer.h"

int main(int argc, char** argv)
{
    RTRenderer rndr;
    rndr.Init();
    rndr.Render();
    rndr.SaveImage("C:/Users/beno.NVIDIA.COM/Desktop/test.jpg");
}