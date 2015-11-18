#version 330

// Input, output and uniforms
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Input
in vec3 vsPos;
in vec3 vsNormal;

// Output
layout(location = 0) out vec4 outFragPosition;
layout(location = 1) out vec4 outFragNormal;
layout(location = 2) out uint outFragMaterialId;

// Uniforms
uniform uint uMaterialId;

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void main()
{
	outFragPosition = vec4(vsPos, 1.0);
	outFragNormal = vec4(vsNormal, 1.0);
	outFragMaterialId = uMaterialId;
}