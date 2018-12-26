#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inTan;

out vec4 passPos;
out vec4 passNorm;
out vec3 passTan;
out vec3 passBitan;
out vec2 passUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    passPos =  vec4(inPos, 1);
    passNorm = vec4(inNorm, 1);
    passTan    = inTan;
    passBitan  = -cross(inTan, inNorm);
    passUV     = inUV;

    gl_Position = proj * view * model * vec4(inPos, 1);
}