#version 400 core

uniform	mat4 ProjectionMatrix;
uniform	mat4 ModelViewMatrix;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTexture;

out vec3 Position;
out vec3 Normal;

void main ()
{

	Position = vec3 (ModelViewMatrix * vec4 (vPosition, 1));
	Normal = vec3 (ModelViewMatrix * vec4 (vNormal, 0));

	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4 (vPosition, 1);
}
