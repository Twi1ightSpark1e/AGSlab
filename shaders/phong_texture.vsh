#version 330 core

layout (std140) uniform PerSceneBlock 
{
	mat4 ProjectionMatrix;
	vec4 lAmbient;
	vec4 lDiffuse;
	vec4 lSpecular;
	vec4 lPosition;
};

layout (std140) uniform PerObjectBlock 
{
	mat4 ModelViewMatrix;
	vec4 mAmbient;
	vec4 mDiffuse;
	vec4 mSpecular;
};

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTexCoord;

out vec3 Position;
out vec3 Normal;

void main ()
{

	Position = vec3 (ModelViewMatrix * vec4 (vPosition, 1));
	Normal = vec3 (ModelViewMatrix * vec4 (vNormal, 0));

	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4 (vPosition, 1);
}
