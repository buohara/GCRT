#pragma once

#include "c3ga.h"
#include "c3ga_util.h"
#include "GCRT.h"

using namespace std;
using namespace c3ga;
using namespace glm;

struct KeyFrame
{
    float t;
    TRversor pose;
    KeyFrame(float tIn, mat4 poseIn);
};

struct Animation
{
    vector<KeyFrame> kfs;
    vector<dualLine> poseLogs;

    void AddKF(KeyFrame &kf);
    mat4 GetAnimationMatrix(float t);
    mat4 GetMatrixFromVersor(TRversor &vers, TRversor &versInv);
};