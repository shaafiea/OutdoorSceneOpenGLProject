// FRAGMENT SHADER
#version 330

in vec4 color;
out vec4 outColor;

in vec4 position;
in vec3 normal;
in vec2 texCoord0;
in mat3 matrixTangent;
vec3 normalNew;

//Fog
uniform vec3 fogColor;
in float fogFactor;

//Rim Light
vec3 V;
vec3 N;
uniform vec3 smoothstep;

uniform sampler2D texture0;
uniform sampler2D textureNormal;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

uniform float att_quadratic;

// View Matrix
uniform mat4 matrixView;

// Light declarations

// Ambient Lights
struct AMBIENT
{	
	int on;
	vec3 color;
};
uniform AMBIENT lightAmbient, lightAmbient1;

vec4 AmbientLight(AMBIENT light)
{
	// Calculate Ambient Light
	return vec4(materialAmbient * light.color, 1);
}

//Directional Light
struct DIRECTIONAL
{	
	int on;
	vec3 direction;
	vec3 diffuse;
};
uniform DIRECTIONAL lightDir;

vec4 DirectionalLight(DIRECTIONAL light)
{
	// Calculate Directional Light
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize(mat3(matrixView) * light.direction);
	float NdotL = dot(normalNew, L);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;
	return color;
}


struct POINT
{
		int on;
		vec3 position;
		vec3 diffuse;
		vec3 specular;
};
uniform POINT lightPoint, lightPoint1, lightPoint2, lightPoint3;


vec4 PointLight(POINT light)
{
	float dist = length(matrixView * vec4(light.position, 1) - position);
	float att = 1 / (att_quadratic * dist * dist);

	// Calculate Point Light (Diffuse)
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize((matrixView) * vec4(light.position, 1) - position).xyz;
	float NdotL = dot(normalNew, L);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	//Point Light(Specular)
	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normalNew);
	float RdotV = dot(R, V);
	if (NdotL > 0 && RdotV > 0)
	    color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);
	return color * att;
}

struct SPOT
{
	mat4 matrix;
	int on;
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	float cutoff; 
	float attenuation;

};
uniform SPOT lightSpot, lightSpot1;

vec4 SpotLight(SPOT light)
{
	// HERE GOES THE CODE COPIED FROM THE POINT LIGHT FUNCTION

  // Calculate Point Light (Diffuse)
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize((light.matrix) * vec4(light.position, 1) - position).xyz;
	float NdotL = dot(normal.xyz, L);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	//Point Light(Specular)
	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal.xyz);
	float RdotV = dot(R, V);
	if (NdotL > 0 && RdotV > 0)
	    color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);

	// HERE GOES THE NEW CODE TO DETERMINE THE SPOT FACTOR
	vec3 D = normalize(mat3(light.matrix) * light.direction).xyz;
	float spotFactor = dot(-L,D);
	float attenuation = acos(spotFactor);

	//Cutoff CarLight
	float cutoff = radians(clamp(light.cutoff, 0, 90));

	if(attenuation <= cutoff)
	{
		spotFactor=pow(spotFactor,light.attenuation);
	}
	else
	{
		spotFactor = 0;
	}

	// assuming that the Point Light value is stored as color and we have calculated spotFactor:
	return spotFactor * color;
}

void main(void) 
{
	normalNew = 2.0 * texture(textureNormal, texCoord0).xyz - vec3(1.0, 1.0, 1.0);
	normalNew = normalize(matrixTangent * normalNew);

	  // calculate light
    outColor = color;
	outColor = vec4(0, 0, 0, 1);
	if (lightAmbient.on == 1)
	{
		outColor += AmbientLight(lightAmbient);
	}
	if (lightAmbient1.on == 1)
	{
		outColor += AmbientLight(lightAmbient1);
	}
	if (lightDir.on == 1)
	{
		outColor += DirectionalLight(lightDir);
	}
    if (lightPoint.on == 1) 
	{
		outColor += PointLight(lightPoint);
	}
	if (lightPoint1.on == 1)
	{
		outColor += PointLight(lightPoint1);
	}
	if (lightPoint2.on == 1)
	{
		outColor += PointLight(lightPoint2);
	}
	if (lightPoint3.on == 1)
	{
		outColor += PointLight(lightPoint3);
	}
	if (lightSpot.on == 1)
	{
		outColor += SpotLight(lightSpot);
	}
	if (lightSpot1.on == 1)
	{
		outColor += SpotLight(lightSpot1);
	}

	outColor *= texture(texture0, texCoord0);
	outColor = mix(vec4(fogColor, 1), outColor, fogFactor);
}