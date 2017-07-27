var renderShaderVS = `#version 300 es
precision highp float;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inTan;

out vec4 passPos;
out vec4 passNorm;
out vec3 passTan;
out vec3 passBitan;
out vec2 passUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    passPos 	= vec4(inPos, 1);
    passNorm 	= vec4(inNorm, 1);
    passTan     = inTan;
    passBitan   = -cross(inTan, inNorm);
    passUV      = inUV;

    gl_Position = proj * view * model * vec4(inPos, 1);
}`;

var renderShaderPS = `#version 300 es
precision highp float;

in vec4 passPos;
in vec4 passNorm;
in vec3 passTan;
in vec3 passBitan;
in vec2 passUV;

uniform mat4 model;
uniform mat4 modelInv;

uniform vec3 kd;
uniform vec3 lightPos;
uniform vec3 lightColor;

out vec4 color;

void main()
{
	vec4 pos        	= model * passPos;
    vec3 lightVec   	= normalize(lightPos - pos.xyz);
    float dist      	= length(lightVec);
	vec3 norm 			= normalize((modelInv * passNorm).xyz);
	float theta 		= max(dot(norm, lightVec), 0.0) / (dist * dist);
	vec4 baseColor 		= vec4(theta * kd * lightColor, 1.0);
	color 				= 1.0 - exp(-5.0 * baseColor);
}`;
