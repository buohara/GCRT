#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

struct Geometry
{
    GLuint vaoID;
    GLuint vertVboID;
    GLuint normVboID;
    GLuint uvVboID;
    GLuint shaderProgID;

    mat4 scl;
    mat4 trans;
    mat4 rot;

    mat4 model;
    mat4 modelInv;
    vec3 kd;
    vec3 ka;

    void SetUniforms();

    virtual void Draw();

    void InitVertexObjects(
        vector<vec3> &verts, 
        vector<vec3> &norms,
        vector<vec2> &uvs
    );

    void InitModelMatrices();
    void Translate(vec3 tx);
    void Scale(vec3 dims);
};