 #version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inUV;

out vec3 exColor;
out vec2 exUV;
out float exTheta;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 modelInv;

uniform vec3 lightPos;
uniform vec3 ka;
uniform vec3 kd;
uniform float ia;
uniform float id;

void main()
{
    vec4 pos = model * vec4(inPos, 1);
    vec4 norm = modelInv * vec4(inNorm, 1);

    vec3 lightVec = normalize(lightPos - pos.xyz);
    float dist = length(lightVec);
    float theta = max(dot(norm.xyz, lightVec), 0) / (dist * dist);

    gl_Position = proj * view * model * vec4(inPos, 1);
    exColor = ia * ka + id * theta * kd;
    exUV = inUV;
    exTheta = theta;
}