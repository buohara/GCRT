#pragma once

#include "GCRT.h"
#include "box.h"
#include "camera.h"
#include "plane.h"
#include "cylinder.h"
#include "sphere.h"

struct Scene
{
    Camera cam;
    GLuint programID;
    bool wireFrame = false;

    Box box1;
    Box box2;
    Plane plane;
    Cylinder cyl;
    Sphere sph;

    void HandleInputs(MSG &msg);
};