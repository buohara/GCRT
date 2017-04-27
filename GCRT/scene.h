#pragma once

#include "GCRT.h"
#include "box.h"
#include "camera.h"
#include "plane.h"
#include "cylinder.h"
#include "sphere.h"
#include "model.h"

struct Scene
{
    Camera cam;
    GLuint programID;
    bool wireFrame = false;

    Model m1;

    void HandleInputs(MSG &msg);
};