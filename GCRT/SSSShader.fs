#version 330 core

in vec4 passPos;
in vec4 passNorm;

out vec4 color;

uniform mat4 lightView;
uniform mat4 lightProj;
uniform sampler2D depthTex;

uniform vec3 lightPos;
uniform vec3 kd;
uniform vec3 camPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 modelInv;

void main()
{
    // Compute diffuse lighting.

    vec4 pos = model * passPos;
    vec4 norm = modelInv * passNorm;
    vec3 norm3 = normalize(norm.xyz);

    vec3 lightVec = normalize(lightPos - pos.xyz);
    float dist = length(lightVec);
    float theta = max(dot(norm3, lightVec), 0);

    // Grab material depth from depth map and compute material tranlucency.

    vec4 posLightSpace = lightProj * lightView * pos;
    
    posLightSpace.x = posLightSpace.x * 0.5 + 0.5;
    posLightSpace.y = posLightSpace.y * 0.5 + 0.5;
    posLightSpace.z = posLightSpace.z * 0.5 + 0.5;

    float shadowDepth = textureProj(depthTex, posLightSpace).z;
    float depth = max(posLightSpace.z  - shadowDepth, 0);
    float bias = 0.005;

    float transluc = exp(-60.0 * depth);

    // Specular term.

    vec3 camVec = normalize(camPos - pos.xyz);
    vec3 rflc = reflect(-camVec, norm3);
    
    float spec = 0.0;

    if (dot(lightVec, norm3) > 0)
    {
        spec = pow(max(dot(camVec, rflc), 0), 16.0) * dot(lightVec, norm3); 
    }

    // Output color.

    color = (theta + spec) * vec4(kd, 1) + transluc * vec4(0.7, 0.3, 0.2, 1.0);
}