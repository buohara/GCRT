#version 330 core

in vec4 passPos;
in vec4 passNorm;
in vec3 passTan;
in vec3 passBitan;
in vec2 passUV;

in mat4 passModel;
in mat4 passModelInv;

out vec4 color;

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
uniform sampler2D envMapTex;

uniform vec3 kd;
uniform float shininess;

uniform int useNormalMap;
uniform int useDiffuseMap;
uniform int useSSS;
uniform int usePhong;
uniform int useShadows;
uniform int useDOF;
uniform int selected;
uniform int useEnvMap;

/**
 * Compute diffuse lighting color.
 */

vec4 getDiffuse()
{
    float theta = 0.0;

    if (usePhong == 1)
    {
        vec4 pos        = passModel * passPos;
        vec3 lightVec   = normalize(lightPos - pos.xyz);
        float dist      = length(lightVec);
        vec3 norm;

        if (useNormalMap == 1)
        {
            mat3 tbn    = mat3(passTan.xyz, passBitan.xyz, passNorm.xyz);
            vec3 nmNorm = tbn * (2 * texture2D(normalTex, passUV).rgb - 1);
            norm        = normalize(passModelInv * vec4(nmNorm, 1)).xyz;
        }
        else norm = normalize((passModelInv * passNorm).xyz);
        theta = max(dot(norm, lightVec), 0) / (dist * dist);
    }
    else theta = 1.0;

    theta = min(1.0, theta + 0.3);

    if (useDiffuseMap == 1) return theta * vec4(texture2D(diffuseTex, passUV).rgb, 1);
    else return theta * vec4(kd, 1);
}

/**
 * Compute specular lighting color.
 */

vec4 getSpecular()
{
    float spec  = 0.0;

    if (usePhong == 1)
    {
        vec4 pos        = passModel * passPos;
        vec3 lightVec   = normalize(lightPos - pos.xyz);
        vec3 norm;

        if (useNormalMap == 1)
        {
            mat3 tbn    = mat3(passTan.xyz, passBitan.xyz, passNorm.xyz);
            vec3 nmNorm = tbn * (2 * texture2D(normalTex, passUV).rgb - 1);
            norm        = normalize(passModelInv * vec4(nmNorm, 1)).xyz;
        }
        else norm = normalize((passModelInv * passNorm).xyz);

        vec3 camVec = normalize(camPos - pos.xyz);
        vec3 rflc   = reflect(-camVec, norm);
    
        if (dot(lightVec, norm) > 0) spec = pow(max(dot(camVec, rflc), 0), shininess) * dot(lightVec, norm);
    }

    if (useDiffuseMap == 1) return spec * vec4(texture2D(diffuseTex, passUV).rgb, 1);
    else return spec * vec4(kd, 1);
}

/**
 * Get visibility based on shadow map.
 */

float getVisibility()
{
    vec4 posLightSpace = lightProj * lightView * passModel * passPos;
    posLightSpace.xyz = posLightSpace.xyz * 0.5 + 0.5;

    float shadowDepth = textureProj(depthTex, posLightSpace).z;
    float depth = posLightSpace.z - shadowDepth;

    float visibility = 1.0;
    float bias = 0.005;

    if (posLightSpace.z - bias > shadowDepth) visibility = 0.25;

    return visibility;
}

/**
 * Get blur factor based on depth and focal plane for DOF.
 */

float getDOFBlur()
{
    float posz = -(view * passModel * passPos).z;
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
 * Determine how far light has traveled through material using
 * depth map.
 */

float getSSSFactor()
{
    vec4 posLightSpace = lightProj * lightView * passModel * passPos;
    posLightSpace.xyz = posLightSpace.xyz * 0.5 + 0.5;

    float shadowDepth = textureProj(depthTex, posLightSpace).z;
    float depth = posLightSpace.z - shadowDepth;

    if (posLightSpace.z - 0.001> shadowDepth)
    {
        return 0.2 * exp(-30 * depth);
    }
    else
    {
        return 0.2;
    }
}

/**
 * SampleEnvMap - Reflect camera ray about normal. Intersect reflected ray with sphere to get UV
 * coordinate of sphere map. Sample sphere map with this UV.
 */

vec4 SampleEnvMap()
{
    vec4 pos    = passModel * passPos;
    vec3 norm   = normalize((passModelInv * passNorm).xyz);
    vec3 camVec = normalize(camPos - pos.xyz);
    vec3 rflc   = normalize(reflect(-camVec, norm));

    float b = 2 * dot(rflc, pos.xyz);
    float c = dot(pos.xyz, pos.xyz) - 500.0 * 500.0;
    float t = (-b + sqrt(b * b - 4.0 *  c)) * 0.5;

    if (t < 0.0) t = (-b - sqrt(b * b - 4.0 * c)) * 0.5;

    vec3 intsc      = pos.xyz + t * rflc;
    float phi       = atan(intsc.y, intsc.x) / 6.18312;
    float theta     = acos(intsc.z / 500.0) / 3.14156;
    vec4 envSample  = texture2D(envMapTex, vec2(phi, theta));

    //vec4 envSample = phi * vec4(1.0, 0.0, 0.0, 1.0);

    return envSample;
}

/**
 * Main fragment shader routine (main render pass).
 */

void main()
{
    vec4 diffuseColor = getDiffuse();
    vec4 specColor    = getSpecular();    
    float visibility  = getVisibility();
    vec4 envMap       = vec4(0.0, 0.0, 0.0, 0.0);

    if (useEnvMap == 1) envMap = SampleEnvMap();
    if (selected == 1) diffuseColor += vec4(0.2, 0.2, 0.2, 0.0);
    if (useSSS == 1) diffuseColor += getSSSFactor();

    if (useEnvMap == 1) color = envMap;
    else color = (diffuseColor + specColor);

    if (useDOF == 1) color.a = getDOFBlur();
}