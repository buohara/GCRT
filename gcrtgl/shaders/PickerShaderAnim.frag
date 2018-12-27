#version 330 core

uniform vec3 pickerColor;
out vec4 color;

void main()
{
    color = vec4(pickerColor, 1);
}