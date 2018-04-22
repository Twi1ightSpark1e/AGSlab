#version 330 core

uniform	mat4 uProjectionMatrix;
uniform	mat4 uModelViewMatrix;

layout(location = 0) in vec3 vPosition;

out vec3 TexCoord;

void main ()
{
	TexCoord = vPosition;

	mat4	OrientMat = uModelViewMatrix;
	OrientMat[3] = vec4(0.0, 0.0, 0.0, 1.0);

	vec4	coord = uProjectionMatrix * OrientMat * vec4(vPosition, 1.0);  

	gl_Position = coord.xyww;
}
