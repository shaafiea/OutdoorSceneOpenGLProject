// VERTEX SHADER

#version 330


//Materials
uniform mat4 matrixProjection;
uniform mat4 matrixModelView;
uniform vec3 materialAmbient;
uniform mat4 matrixView;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

layout(location = 0) in vec3 aVertex;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBiTangent;


out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;
out mat3 matrixTangent;

// FOG
out float fogFactor;
uniform float fogDensity;


void main(void) 
{
  position = matrixModelView * vec4(aVertex, 1.0);
  gl_Position = matrixProjection * position;

  //calculate normal
  normal = normalize(mat3(matrixModelView) * aNormal);

  //Texture
  texCoord0 = aTexCoord;

  // calculate tangent local system transformation
  vec3 tangent = normalize(mat3(matrixModelView) * aTangent);
  vec3 biTangent = normalize(mat3(matrixModelView) * aBiTangent);
  matrixTangent = mat3(tangent, biTangent, normal);


  fogFactor = exp2(-fogDensity * length(position));
}
