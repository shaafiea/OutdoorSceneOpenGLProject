#version 330

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;

// Output Variable (sent down through the Pipeline)
out vec4 outColor;

// Water-related
uniform vec4 waterColor;
uniform vec4 skyColor;

in float reflFactor;			// reflection coefficient

//Fog
uniform vec3 fogColour;
in float fogFactor;

void main(void) 
{
	outColor = color;
	outColor = mix(waterColor, skyColor, reflFactor);
	outColor = mix(vec4(fogColour, 1), outColor, fogFactor);
}
