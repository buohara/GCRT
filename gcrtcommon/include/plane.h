#pragma once

#include "gcrtcommon.h"
using namespace std;
using namespace glm;

void GenPositionsPlane(vector<vec3>& pos, uint32_t rows, uint32_t cols);
void GenNormalsPlane(vector<vec3>& norms, uint32_t rows, uint32_t cols);
void GenUVsPlane(vector<vec2>& uvs, uint32_t rows, uint32_t cols);
void GenTangentsPlane(vector<vec3>& tans, uint32_t rows, uint32_t cols);