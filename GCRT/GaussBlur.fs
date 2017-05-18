#version 330 core

in vec2 passUV;
out vec4 color;

uniform sampler2D colorTex;
uniform int horizontal;

void main()
{
    float weights[5] = float[](0.22, 0.19, 0.12, 0.05, 0.02);
    vec2 texSize = 1.0 / textureSize(colorTex, 0);
    
    vec4 result = texture2D(colorTex, passUV) * weights[0];

    if (horizontal == 1)
    {
        for (int i = 1; i < 5; i++)
        {
            result += texture2D(colorTex, passUV + i * vec2(texSize.x, 0.0)) * weights[i];
            result += texture2D(colorTex, passUV - i * vec2(texSize.x, 0.0)) * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            result += texture2D(colorTex, passUV + i * vec2(0.0, texSize.y)) * weights[i];
            result += texture2D(colorTex, passUV - i * vec2(0.0, texSize.y)) * weights[i];
        }
    }

    color = result;
}