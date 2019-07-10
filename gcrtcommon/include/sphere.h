#pragma once

#include "gcrt.h"
using namespace std;
using namespace glm;

void GenPositionsSphere(
    vector<vec3>& pos,
    uint32_t numSectors,
    uint32_t numRings,
    uint32_t& numSideVerts,
    uint32_t& topOffset,
    uint32_t& bottomOffset,
    uint32_t& numCapVerts
);

void GenNormalsSphere(vector<vec3>& norms, uint32_t numSectors, uint32_t numRings);
void GenUVsSphere(vector<vec2>& uvs, uint32_t numSectors, uint32_t numRings);
void GenTansSphere(vector<vec3>& tans, uint32_t numSectors, uint32_t numRings);