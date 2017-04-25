#pragma once

#define GLEW_STATIC

#include <Windows.h>
#include <glew.h>
#include <wglew.h>
#include <gl/GL.h>
#include <stdio.h>

#include "camera.h"
#include "box.h"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Scene
{
    Camera cam;
    GLuint programID;
    bool wireFrame;

    Box box1;
    Box box2;

    void HandleInputs(MSG &msg);
};