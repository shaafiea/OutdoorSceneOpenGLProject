#version 330

in float age;
uniform sampler2D texture0;
out vec4 outColor;

//Fog
in float distanceFactor;
in vec4 position;

void main()
{
    outColor = vec4(0, 0.5, 1.0, 1 - age);
    outColor = texture(texture0, gl_PointCoord);
	outColor.a = 1 - outColor.r * outColor.g * outColor.b;
    outColor.a *= 1 - age;

}

