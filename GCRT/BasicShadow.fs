#version 330 core

in vec4 passColor;
in vec4 passPos;

out vec4 color;

uniform mat4 model;
uniform mat4 lightView;
uniform mat4 lightProj;
uniform sampler2D depthTex;

void main()
{
    vec4 posLightSpace = lightProj * lightView * model * passPos;
    posLightSpace.x = posLightSpace.x * 0.5 + 0.5;
    posLightSpace.y = posLightSpace.y * 0.5 + 0.5;
    posLightSpace.z = posLightSpace.z * 0.5 + 0.5;

    float shadowDepth = textureProj(depthTex, posLightSpace).z;

    float visibility = 2.0;

    if (posLightSpace.z - 0.005 > shadowDepth)
    {
        visibility = 0.5;
    }

    color = visibility * passColor;
}