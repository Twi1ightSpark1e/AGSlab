#version 330 core

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform vec4 Color;

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

void main()
{
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4 (vPosition, 1);
}
