#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

float IntersectPlane(
    vec3 org,
    vec3 ray,
    vec3 p,
    vec3 norm,
    mat4 modelInv,
    vec3 checkBounds
);

struct Geometry
{
    GLuint vaoID;

    GLuint posVboID;
    GLuint normVboID;
    GLuint uvVboID;
    GLuint tanVboID;

    mat4 scl;
    mat4 trans;
    mat4 rot;

    mat4 model;
    mat4 modelInv;

    virtual void Draw() = 0;
    virtual float Intersect(vec3 org, vec3 ray) = 0;

    void InitVertexObjects(
        vector<vec3> &pos,
        vector<vec3> &norms,
        vector<vec2> &uvs
    );

    void InitVertexObjects(
        vector<vec3> &pos, 
        vector<vec3> &norms,
        vector<vec2> &uvs,
        vector<vec3> &tans
    );

    void InitModelMatrices();
    void Translate(vec3 tx);
    void Scale(vec3 dims);
};