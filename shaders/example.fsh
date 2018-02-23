#version 330 core

uniform	vec4 Offset;
uniform	vec4 Color;

in float gradient;

void main (void)
{
	gl_FragColor = vec4(Color) * gradient +  vec4(0.0, 0.0, 0.0, 1.0) * (1.0 - gradient);
}
