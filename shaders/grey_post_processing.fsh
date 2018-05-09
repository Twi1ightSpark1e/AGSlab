#version 330 core

uniform	sampler2D tex;

in vec2 TexCoord;

void main (void)
{
	vec4	TexColor = texture (tex, TexCoord);
	float	Luminance = 0.2126 * TexColor.r + 0.7152 * TexColor.g + 0.0722 * TexColor.b;
	gl_FragColor = vec4(Luminance, Luminance, Luminance, 1.0);
}
