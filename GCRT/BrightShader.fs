#version 330 core

in vec2 passUV;
out vec4 color;

uniform sampler2D colorTex;

void main()
{
    vec3 inColor = texture2D(colorTex, passUV).rgb;
    
    if (dot(inColor, vec3(0.21, 0.72, 0.07)) > 0.9)
    {
        color = vec4(inColor, 1.0);
    }
    else
    {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}