#version 330

in vec3 normal;
smooth in vec4 theColor;
out vec4 outputColor;

in vec3 position;
in vec3 vN;

in vec4 SpecularColor;
in float SpecularExponent;

vec3 CalcLVector(vec4 source_position, vec3 pos)
{
	return normalize(source_position.xyz - pos.xyz);
}

vec3 HalfAngle(vec3 l, vec3 e)
{
	return normalize(l + e);
}

vec4 Diffuse(vec3 l, vec3 n, vec4 clr, vec4 direction)
{
	vec3 d = normalize(direction).xyz;
	float co = cos(3.14);

	if (dot(-l,d) >  co) 
		return max(dot(l,n),0.0) * theColor * clr;
	else
		return vec4(0,0,0,1);
}

vec4 Specular(vec3 n, vec3 h, vec4 clr,vec3 l, vec4 direction)
{
	vec3 d = normalize(direction).xyz;
	float co = cos(3.14);

	if (dot(-l,d) >  co) 
		return pow(max(dot(n,h),0.0),300) * SpecularColor * clr;
	else
		return vec4(0,0,0,1);	
}

void main()
{
		vec4 ambient_light = vec4(1.0,1.0,1.0,0.8);

		vec4 lighColor = vec4(1.0,1.0,1.0,0.5);
		vec4 lightdirection = vec4(-1,0,1,0);
		vec4 lightposition = vec4(1000,1000,1000,0);

		vec3 N = normalize(vN.xyz);
		vec3 E = -normalize(position.xyz);
		vec3 L = CalcLVector(lightposition, position);
		vec3 H = HalfAngle(L,E);

		vec4 diff = Diffuse(L,N,lighColor,lightdirection);
		vec4 spec = Specular(N,H,lighColor,L,lightdirection);

		float	f = dot(L,N);

		if (f < 0.0)
		{
			spec = vec4(0,0,0,1);
		}

		// outputColor = theColor;

		//outputColor = diff + spec + ambient_light;

		outputColor = diff + ambient_light;
		
		// outputColor = vec4(1,1,1,1);
}