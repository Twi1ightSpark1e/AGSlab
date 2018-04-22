#version 400 core

uniform	mat4 ProjectionMatrix;
uniform	mat4 ModelViewMatrix;
uniform	vec4 Color;

layout(location = 0) in vec3 vPosition;

void main ()
{
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4 (vPosition, 1);
}
