#version 330 core

layout(location = 0) in vec2 vPosition;

out vec2 TexCoord;

void main ()
{
	TexCoord = vPosition * 0.5f + vec2(0.5f, 0.5f); 
	gl_Position = vec4 (vPosition, 0, 1);
}
