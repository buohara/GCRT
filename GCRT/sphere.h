#pragma once

#include <glew.h>
#include <gl/GL.h>
#include <stdio.h>
#include <vector>
#include <math.h>

#include "glm/glm.hpp"

using namespace std;
using namespace glm;

struct Sphere
{
    vector<vec3> verts;
    vector<uint32_t> idcs;
    GLuint vbo;
    void Create(vec3 ctr, vec3 rad, uint32_t rings, uint32_t segments);
};