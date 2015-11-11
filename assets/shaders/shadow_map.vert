#version 330

in vec3 inPosition;
in vec3 inNormal;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix; // inverse(transpose(modelViewMatrix)) for non-uniform scaling

void main()
{
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(inPosition, 1);
}