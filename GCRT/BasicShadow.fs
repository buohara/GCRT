#version 330 core

in vec4 passColor;
in vec4 passPos;
in float passTheta;

out vec4 color;

uniform mat4 lightView;
uniform mat4 lightProj;
uniform sampler2D depthTex;

void main()
{
    vec4 posLightSpace = lightProj * lightView * passPos;
    
    posLightSpace.x = posLightSpace.x * 0.5 + 0.5;
    posLightSpace.y = posLightSpace.y * 0.5 + 0.5;
    posLightSpace.z = posLightSpace.z * 0.5 + 0.5;

    float shadowDepth = textureProj(depthTex, posLightSpace).z;
    float visibility = 2.0;
    float bias = 0.005 * tan(acos(passTheta));

    if (posLightSpace.z - bias > shadowDepth)
    {
        visibility = 0.5;
    }

    color = vec4(0.2, 0.2, 0.2, 1.0) + visibility * passColor;
}