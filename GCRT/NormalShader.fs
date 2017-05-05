 #version 330 core

in vec4 passPos;
in vec3 passNorm;
in vec3 passTan;
in vec3 passBitan;
in vec2 passUV;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 camPos;
uniform mat4 modelInv;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

void main()
{
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

    color = theta * vec4(texture2D(diffuseTex, passUV).rgb, 1);
}