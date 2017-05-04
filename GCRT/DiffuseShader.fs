#version 330 core

in vec3 exColor;
in vec2 exUV;
in float exTheta;
out vec4 color;

uniform sampler2D texture;

void main()
{
    color = vec4(exColor, 1.0) + 1.5 * exTheta * vec4(texture2D(texture, exUV).rgb, 1);
}