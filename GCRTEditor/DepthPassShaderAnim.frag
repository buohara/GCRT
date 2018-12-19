#version 330 core

layout (location = 0) out vec4 depthOut;

void main()
{
    depthOut = vec4(0, 0, 0, 1);
}