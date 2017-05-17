#version 330 core

in vec2 passUV;

out vec4 color;

uniform sampler2D colorTex;
uniform sampler2D noiseTex;
uniform vec2 samplePts[16];

void main()
{
    float depth = texture2D(colorTex, passUV).a;
    float blur = abs(depth) / 2;
    blur = blur * blur;

    float dtheta = texture2D(noiseTex, passUV).r;

    for (int i = 0; i < 8; i++)
    {
        vec2 sampleUV = passUV;

        float r = samplePts[i].x;
        float theta = samplePts[i].y;

        float dx = r * cos(theta + dtheta);
        float dy = r * sin(theta + dtheta);

        sampleUV.x += 0.1 * blur * dx;
        sampleUV.y += 0.1 * blur * dy;

        float sampleDepth = texture2D(colorTex, sampleUV).a;
        
        if (sampleDepth < depth)
        {
            color += texture2D(colorTex, passUV) / 8.0;
        }
        else
        {
            color += texture2D(colorTex, sampleUV) / 8.0;
        }
    }
}