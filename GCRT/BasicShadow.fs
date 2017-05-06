#version 330 core

in vec4 passColor;
out vec4 color;

void main()
{
    color = vec4(0.2, 0.2, 0.2, 1) + passColor;
}