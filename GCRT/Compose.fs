#version 330 core

in vec2 passUV;

uniform sampler2D colorTex;
uniform sampler2D blurTex;

out vec4 color;

void main()
{
    vec3 sceneColor = texture2D(blurTex, passUV).rgb;
    vec3 blurColor = texture2D(colorTex, passUV).rgb;

    sceneColor += blurColor;

    vec3 result = vec3(1.0) - exp(-sceneColor);
    result = pow(result, vec3(1.0 / 2.2));
    color = vec4(result, 1);
}