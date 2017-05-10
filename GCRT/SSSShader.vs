#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;

out vec4 passPos;
out vec4 passNorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    passPos =  vec4(inPos, 1);
    passNorm = vec4(inNorm, 1);

    gl_Position = proj * view * model * vec4(inPos, 1);
}