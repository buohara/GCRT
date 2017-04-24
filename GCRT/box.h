#pragma once

#include <glew.h>
#include <gl/GL.h>
#include <stdio.h>
#include <vector>
#include <math.h>

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_inverse.hpp"

using namespace std;
using namespace glm;

struct Box
{
    uint32_t numVerts;

    GLuint vaoID;
    GLuint vertVboID;
    GLuint normVboID;
    GLuint shaderProgID;
    
    mat4 model;
    mat4 modelInv;

    vec3 clr;

    void Create(vec3 dims, vec3 pos, vec3 color, GLuint progID);
    void Draw();
};