#version 330

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec3 vNormal;


uniform mat4 model_view;
uniform mat4 projection;
uniform int intersect_index;

out vec3 position;
out vec3 vN;
out vec4 SpecularColor;
out float SpecularExponent;

smooth out vec4 theColor;

void main()
{
	vec4 vNormal = vec4(-vNormal,0);
	vec4 veyepos = model_view * vPosition;
	SpecularColor		=	vec4(1,1,1,1);
	SpecularExponent	=	2;
	
	vN					= normalize(model_view * vNormal).xyz;

	position			= veyepos.xyz;

	gl_Position =	projection * model_view * vPosition;
	//gl_Position = vPosition;
	
	theColor = vColor;
	

}