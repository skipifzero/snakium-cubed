#version 330

in vec3 inPosition;

uniform mat4 uViewProjMatrix;
uniform mat4 uModelMatrix;

void main()
{
	gl_Position = uViewProjMatrix * uModelMatrix * vec4(inPosition, 1);
}