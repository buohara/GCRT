#pragma once

#include "gcrtcommon.h"
using namespace std;
using namespace glm;

void GenPositionsCylinder(
    vector<vec3>& pos,
    uint32_t numSectors,
    uint32_t& numSideVerts,
    uint32_t& topOffset,
    uint32_t& bottomOffset,
    uint32_t& numCapVerts
);

void GenNormalsCylinder(vector<vec3>& norms, uint32_t numSectors);
void GenUVsCylinder(vector<vec2>& uvs, uint32_t numSectors);
void GenTansCylinder(vector<vec3>& tans, uint32_t numSectors);