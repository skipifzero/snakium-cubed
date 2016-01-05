#version 330

// Input, output and uniforms
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Input
in vec3 vsPos;
in vec3 vsNormal;

// Uniforms
uniform float uFarPlaneDist;

// Output
layout(location = 0) out vec4 outFragLinearDepth;
layout(location = 1) out vec4 outFragNormal;
layout(location = 2) out uint outFragMaterialId;

// Uniforms
uniform uint uMaterialId;

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void main()
{
	outFragLinearDepth = vec4(-vsPos.z / uFarPlaneDist, 0.0, 0.0, 1.0);
	outFragNormal = vec4(vsNormal, 1.0);
	outFragMaterialId = uMaterialId;
}