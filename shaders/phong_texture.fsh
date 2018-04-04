#version 400 core

layout (std140) uniform PerSceneBlock // binding = 0
{
	mat4 ProjectionMatrix;
	vec4 lAmbient;
	vec4 lDiffuse;
	vec4 lSpecular;
	vec4 lPosition;
};

layout (std140) uniform PerObjectBlock // binding = 1
{
	mat4 ModelViewMatrix;
	vec4 mAmbient;
	vec4 mDiffuse;
	vec4 mSpecular;
};

in vec3 Position;
in vec3 Normal;

void main (void)
{
	vec3	n_Normal = normalize(Normal);
	vec3	n_ToLight = normalize(vec3(lPosition));
	vec3	n_ToEye = normalize (vec3(0,0,0) - Position);
	vec3	n_Reflect = normalize(reflect(-n_ToLight,n_Normal));
	
	vec4	Ambient = mAmbient * lAmbient;  
	vec4	Diffuse = mDiffuse * lDiffuse * max(dot(n_Normal, n_ToLight), 0.0f);
	vec4	Specular = mSpecular * lSpecular * pow(max(dot(n_ToEye, n_Reflect), 0.0f), 64.0f);

	vec3	Color = vec3(Ambient + Diffuse + Specular); 
	float	alpha = mDiffuse.a;

	gl_FragColor = vec4(Color,1.0);
}
