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

// Input: Water Related
in float waterDepth;		// water depth (positive for underwater, negative for the shore)

//Textures
uniform sampler2D textureBed; // Below water Level Texture
uniform sampler2D textureShore; // Above water Level Texture

//Fog
uniform vec3 fogColour;
in float fogFactor;

void main(void) 
{
	outColor = color;
	// shoreline multitexturing
	float isAboveWater = clamp(-waterDepth, 0, 1); 
	outColor *= mix(texture(textureBed, texCoord0), texture(textureShore, texCoord0), isAboveWater);
	outColor = mix(vec4(fogColour, 1), outColor, fogFactor);
}
