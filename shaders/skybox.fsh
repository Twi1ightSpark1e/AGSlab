#version 330 core

uniform samplerCube tex;

in vec3 TexCoord;

void main (void)
{
	vec4	TexColor = texture (tex,TexCoord);
	gl_FragColor = TexColor;
}
