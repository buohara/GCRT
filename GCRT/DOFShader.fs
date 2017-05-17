#version 330 core

in vec2 passUV;

out vec4 color;

uniform sampler2D colorTex;
uniform vec2 samplePts[16];

void main()
{
    float depth = texture2D(colorTex, passUV).a;
    float blur = abs(depth) / 2;
    blur = blur * blur;

    //color = vec4(blur, blur, blur, 1);
    //return;

    for (int i = 0; i < 16; i++)
    {
        vec2 sampleUV = passUV;
        sampleUV.x += 0.08 * blur * samplePts[i].x;
        sampleUV.y += 0.05 * blur * samplePts[i].y;

        float sampleDepth = texture2D(colorTex, sampleUV).a;
        float sampleBlur = abs(sampleDepth) / 3;
        sampleBlur = sampleBlur * sampleBlur;

        if (sampleDepth < depth && sampleBlur < 0.02)
        {
            color += texture2D(colorTex, passUV) / 16.0;
        }
        else
        {
            color += texture2D(colorTex, sampleUV) / 16.0;
        }
    }
}