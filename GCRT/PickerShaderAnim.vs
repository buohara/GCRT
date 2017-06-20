#version 330 core

layout(location = 0) in vec3 inPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout(location = 4) in ivec4 inBoneIDs;
layout(location = 5) in vec4 inBoneWeights;

const int MAX_BONES = 64;
uniform mat4 bones[MAX_BONES];

void main()
{
    mat4 boneTransform = inBoneWeights[0] * bones[inBoneIDs[0]];
    boneTransform += inBoneWeights[1] * bones[inBoneIDs[1]];
    boneTransform += inBoneWeights[2] * bones[inBoneIDs[2]];
    boneTransform += inBoneWeights[3] * bones[inBoneIDs[3]];

    gl_Position = proj * view * boneTransform * vec4(inPos, 1);
}