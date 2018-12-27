#pragma once

#include "GCRT.h"

using namespace std;
using namespace glm;

struct KeyFrame
{
    float t;
    quat rot;
    vec3 trans;
    vec3 scale;

    KeyFrame(float tIn, quat rotIn, vec3 transIn, vec3 scaleIn) :
        t(tIn), rot(rotIn), trans(transIn), scale(scaleIn)
    {}
};

struct Animation
{
    vector<KeyFrame> kfs;
    void AddKF(KeyFrame &kf);
    mat4 GetAnimationMatrix(float t);
};