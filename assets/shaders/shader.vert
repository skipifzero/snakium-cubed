#version 330

in vec3 inPosition;
in vec3 inNormal;
in int inMaterialID;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix; // inverse(transpose(modelViewMatrix)) for non-uniform scaling

out vec3 vsPos;
out vec3 vsNormal;

void main()
{
	mat4 modelViewMatrix = uViewMatrix * uModelMatrix;
	vsPos = (modelViewMatrix * vec4(inPosition, 1)).xyz;
	vsNormal = normalize((uNormalMatrix * vec4(inNormal, 0)).xyz);
	gl_Position = uProjMatrix * modelViewMatrix * vec4(inPosition, 1);
}