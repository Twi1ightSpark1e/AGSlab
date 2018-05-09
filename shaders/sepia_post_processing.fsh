#version 330 core

uniform	sampler2D tex;

in vec2 TexCoord;

void main (void)
{
	vec4	TexColor = texture (tex, TexCoord);
	gl_FragColor.r = 0.393 * TexColor.r + 0.769 * TexColor.g + 0.189 * TexColor.b; 
	gl_FragColor.g = 0.349 * TexColor.r + 0.686 * TexColor.g + 0.168 * TexColor.b; 
	gl_FragColor.b = 0.272 * TexColor.r + 0.534 * TexColor.g + 0.131 * TexColor.b; 
	gl_FragColor.a = 1.0;
}
