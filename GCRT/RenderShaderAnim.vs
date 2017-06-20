#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inTan;
layout(location = 4) in ivec4 inBoneIDs;
layout(location = 5) in vec4 inBoneWeights;

const int MAX_BONES = 64;

out vec4 passPos;
out vec4 passNorm;
out vec3 passTan;
out vec3 passBitan;
out vec2 passUV;

uniform bool animated;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 bones[MAX_BONES];

void main()
{
    passPos     = vec4(inPos, 1);
    passNorm    = vec4(inNorm, 1);
    passTan     = inTan;
    passBitan   = -cross(inTan, inNorm);
    passUV      = inUV;

    mat4 boneTransform = inBoneWeights[0] * bones[inBoneIDs[0]];
    boneTransform += inBoneWeights[1] * bones[inBoneIDs[1]];
    boneTransform += inBoneWeights[2] * bones[inBoneIDs[2]];
    boneTransform += inBoneWeights[3] * bones[inBoneIDs[3]];

    gl_Position = proj * view * boneTransform * vec4(inPos, 1);
}