#version 330

in vec3 inPosition;
in vec3 inNormal;
in int inMaterialID;

uniform mat4 uModelViewMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormalMatrix; // inverse(transpose(modelViewMatrix)) for non-uniform scaling

out vec3 vsPos;
out vec3 vsNormal;

void main()
{
	vsPos = (uModelViewMatrix * vec4(inPosition, 1)).xyz;
	vsNormal = (uNormalMatrix * vec4(inNormal, 0)).xyz;
	gl_Position = uProjMatrix * uModelViewMatrix * vec4(inPosition, 1);
}