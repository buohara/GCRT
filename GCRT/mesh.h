#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

struct Mesh
{
    GLuint vaoID;

    GLuint posVboID;
    GLuint normVboID;
    GLuint uvVboID;
    GLuint tanVboID;

    virtual void Draw() = 0;

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
};