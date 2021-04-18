#version 330

// Uniforms: Transformation Matrices
uniform mat4 matrixProjection;
uniform mat4 matrixView;
uniform mat4 matrixModelView;

// Uniforms: Material Colours
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;

in vec3 aVertex;
in vec3 aNormal;
in vec2 aTexCoord;

out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;

//Normal Map Attributes
in vec3 aTangent;
in vec3 aBiTangent;
out mat3 matrixTangent;

// Uniforms: Water Related
uniform float waterLevel;	// water level (in absolute units)

// Output: Water Related
out float waterDepth;

// FOG
out float fogFactor;
uniform float fogDensity;

void main(void) 
{
	// calculate position
	position = matrixModelView * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;

	// calculate normal
	normal = normalize(mat3(matrixModelView) * aNormal);

	// calculate texture coordinate
	texCoord0 = aTexCoord;

	// calculate depth of water
	waterDepth = waterLevel - aVertex.y;


	// calculate the observer's altitude above the observed vertex
	float eyeAlt = dot(-position.xyz, mat3(matrixModelView) * vec3(0, 1, 0));

	// calculate tangent local system transformation
	vec3 tangent = normalize(mat3(matrixModelView) * aTangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);	// Gramm-Schmidt process
	vec3 biTangent = cross(normal, tangent);
	matrixTangent = mat3(tangent, biTangent, normal);

	//calculate fog factor
	fogFactor = exp2(-fogDensity * length(position) *  (max(waterDepth, 0) / eyeAlt));
}
