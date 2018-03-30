#version 400 core

uniform	vec4 mAmbient;
uniform	vec4 mDiffuse;
uniform	vec4 mSpecular;

uniform	vec4 lAmbient;
uniform	vec4 lDiffuse;
uniform	vec4 lSpecular;
uniform	vec4 lPosition;
uniform float SpecularPow;

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
	vec4	Specular = mSpecular * lSpecular * pow(max(dot(n_ToEye, n_Reflect), 0.0f), SpecularPow);

	vec3	Color = vec3(Ambient + Diffuse + Specular); 
	float	alpha = mDiffuse.a;

	gl_FragColor = vec4(Color,1.0);
}
