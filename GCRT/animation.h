#pragma once

#include "c3ga.h"
#include "c3ga_util.h"
#include "GCRT.h"

using namespace std;
using namespace glm;

struct KeyFrame
{
    float t;
    c3ga::TRversor pose;
    mat4 poseMat;
    KeyFrame(float tIn, mat4 poseIn);
};

struct Animation
{
    vector<KeyFrame> kfs;
    vector<c3ga::dualLine> poseLogs;

    void AddKF(KeyFrame &kf);
    mat4 GetAnimationMatrix(float t);
    mat4 GetMatrixFromVersor(c3ga::TRversor &vers, c3ga::TRversor &versInv);
};