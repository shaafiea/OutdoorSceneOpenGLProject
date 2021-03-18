// FRAGMENT SHADER
#version 330

in vec4 color;
out vec4 outColor;

in vec4 position;
in vec3 normal;
in vec2 texCoord0;

//Fog
uniform vec3 fogColour;
in float fogFactor;


uniform sampler2D texture0;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

uniform float att_quadratic;

// View Matrix
uniform mat4 matrixView;

struct POINT
{
		int on;
		vec3 position;
		vec3 diffuse;
		vec3 specular;
};
uniform POINT lightPoint, lightPoint1, lightPoint2;


vec4 PointLight(POINT light)
{
	float dist = length(matrixView * vec4(light.position, 1) - position);
	float att = 1 / (att_quadratic * dist * dist);

	// Calculate Point Light (Diffuse)
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize((matrixView) * vec4(light.position, 1) - position).xyz;
	float NdotL = dot(normal, L);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	//Point Light(Specular)
	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal);
	float RdotV = dot(R, V);
	if (NdotL > 0 && RdotV > 0)
	    color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);
	return color * att;
}

void main(void) 
{
    outColor = color;
    if (lightPoint.on == 1) 
		outColor += PointLight(lightPoint);
	if (lightPoint1.on == 1) 
		outColor += PointLight(lightPoint1);
	if (lightPoint2.on == 1) 
		outColor += PointLight(lightPoint2);

	outColor *= texture(texture0, texCoord0);
	outColor = mix(vec4(fogColour, 1), outColor, fogFactor);
}