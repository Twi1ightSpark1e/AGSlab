#version 330 core

uniform	sampler2D tex;

layout(location = 0) out vec3 color;
in vec2 TexCoord;

void main (void)
{
	color = texture (tex, TexCoord).xyz;
}
