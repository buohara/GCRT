#version 330 core

in vec4 passPos;
in vec4 passNorm;
in vec3 passTan;
in vec3 passBitan;
in vec2 passUV;

out vec4 color;

// Geometry uniforms

uniform mat4 model;
uniform mat4 modelInv;

// Camera uniforms

uniform mat4 view;
uniform mat4 proj;
uniform vec3 camPos;
uniform vec4 focusPlanes;

// Light uniforms

uniform vec3 lightPos;
uniform mat4 lightView;
uniform mat4 lightProj;
uniform vec3 ka;

// Material uniforms

uniform sampler2D depthTex;
uniform sampler2D normalTex;
uniform sampler2D diffuseTex;

uniform vec3 kd;
uniform float shininess;

uniform int useNormalMap;
uniform int useDiffuseMap;
uniform int useSSS;
uniform int useShadows;
uniform int useDOF;
uniform int selected;

/**
 * Compute diffuse lighting color.
 */

vec4 getDiffuse()
{
    vec4 pos        = model * passPos;
    vec3 lightVec   = normalize(lightPos - pos.xyz);
    float dist      = length(lightVec);
    vec3 norm;

    if (useNormalMap == 1)
    {
        mat3 tbn    = mat3(passTan.xyz, passBitan.xyz, passNorm.xyz);
        vec3 nmNorm = tbn * (2 * texture2D(normalTex, passUV).rgb - 1);
        norm        = normalize(modelInv * vec4(nmNorm, 1)).xyz;
    }
    else
    {
        norm = normalize((modelInv * passNorm).xyz);
    }

    float theta = max(dot(norm, lightVec), 0) / (dist * dist);

    if (useDiffuseMap == 1)
    {
        return theta * vec4(texture2D(diffuseTex, passUV).rgb, 1);
    }
    else
    {
        return theta * vec4(kd, 1);
    }
}

/**
 * Compute specular lighting color.
 */

vec4 getSpecular()
{
    vec4 pos        = model * passPos;
    vec3 lightVec   = normalize(lightPos - pos.xyz);
    vec3 norm;

    if (useNormalMap == 1)
    {
        mat3 tbn    = mat3(passTan.xyz, passBitan.xyz, passNorm.xyz);
        vec3 nmNorm = tbn * (2 * texture2D(normalTex, passUV).rgb - 1);
        norm        = normalize(modelInv * vec4(nmNorm, 1)).xyz;
    }
    else
    {
        norm = normalize((modelInv * passNorm).xyz);
    }

    vec3 camVec = normalize(camPos - pos.xyz);
    vec3 rflc   = reflect(-camVec, norm);
    float spec  = 0.0;

    if (dot(lightVec, norm) > 0)
    {
        spec = pow(max(dot(camVec, rflc), 0), shininess) * dot(lightVec, norm); 
    }

    if (useDiffuseMap == 1)
    {
        return spec * vec4(texture2D(diffuseTex, passUV).rgb, 1);
    }
    else
    {
        return spec * vec4(kd, 1);
    }
}

/**
 * Get visibility based on shadow map.
 */

float getVisibility()
{
    vec4 posLightSpace = lightProj * lightView * model * passPos;
    posLightSpace.xyz = posLightSpace.xyz * 0.5 + 0.5;

    float shadowDepth = textureProj(depthTex, posLightSpace).z;
    float depth = posLightSpace.z - shadowDepth;

    float visibility = 1.0;
    float bias = 0.005;

    if (posLightSpace.z - bias > shadowDepth)
    {
        visibility = 0.25;
    }

    return visibility;
}

/**
 * Get blur factor based on depth and focal plane for DOF.
 */

float getDOFBlur()
{
    float posz = -(view * model * passPos).z;
    float blur = 0.0;

    if (posz < focusPlanes.y)
    {
        blur = (focusPlanes.y - posz) / (focusPlanes.y - focusPlanes.x);
    }
    else if (posz > focusPlanes.y && posz < focusPlanes.z)
    {
        blur = 0.0;
    }
    else
    {
        blur = (posz - focusPlanes.z) / (focusPlanes.w - focusPlanes.z);
    }

    return blur;  
}

/**
 * Main fragment shader routine (main render pass).
 */

void main()
{    
    vec4 diffuseColor = getDiffuse();
    vec4 specColor    = getSpecular();    
    float visibility  = getVisibility();

    if (selected == 1)
    {
        diffuseColor += vec4(0.2, 0.2, 0.2, 0.0);
    }

    color = visibility * (diffuseColor + specColor);

    if (useDOF == 1)
    {
        color.a = getDOFBlur();
    }
}