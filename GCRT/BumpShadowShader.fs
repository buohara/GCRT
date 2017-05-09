 #version 330 core

in vec4 passPos;
in vec3 passNorm;
in vec3 passTan;
in vec3 passBitan;
in vec2 passUV;

out vec4 color;

uniform vec3 lightPos;
uniform mat4 lightView;
uniform mat4 lightProj;

uniform vec3 camPos;
uniform mat4 modelInv;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D depthTex;

void main()
{
    // Compute pixel normal from normal map and tangent space.

    mat3 tbn    = mat3(passTan.xyz, passBitan.xyz, passNorm.xyz);
    vec3 nmNorm = tbn * (2 * texture2D(normalTex, passUV).rgb - 1);
    vec4 norm   = modelInv * vec4(nmNorm, 1);

    vec3 lightVec = normalize(lightPos - passPos.xyz);
    float dist    = length(lightVec);
    float theta   = max(dot(normalize(norm.xyz), lightVec), 0) / (dist * dist);

    vec3 camVec = normalize(camPos - passPos.xyz);
    float spec = 0.0;

    if (dot(lightVec, norm.xyz) > 0)
    {
        vec3 rflct = normalize(reflect(lightVec, norm.xyz));
        spec = pow(max(dot(rflct, camVec), 0), 32);
    }

    // Compute visibility from shadow map.

    vec4 posLightSpace = lightProj * lightView * passPos;
    
    posLightSpace.x = posLightSpace.x * 0.5 + 0.5;
    posLightSpace.y = posLightSpace.y * 0.5 + 0.5;
    posLightSpace.z = posLightSpace.z * 0.5 + 0.5;

    float shadowDepth = textureProj(depthTex, posLightSpace).z;
    float visibility = 2.0;
    float bias = 0.005 * tan(acos(theta));

    if (posLightSpace.z - bias > shadowDepth)
    {
        visibility = 0.5;
    }

    color = vec4(0.2, 0.2, 0.2, 1) + visibility * theta * vec4(texture2D(diffuseTex, passUV).rgb, 1);
}