#version 330

// Structs
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emissive;
	float shininess;
	float opaque;
};

// Input, output and uniforms
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Input
in vec3 vsPos;
in vec3 vsNormal;

// Output
layout(location = 0) out vec4 outFragPosition;
layout(location = 1) out vec4 outFragNormal;
layout(location = 2) out vec4 outFragEmissive;
layout(location = 3) out unsigned int outFragMaterialId;

// Uniforms
uniform Material uMaterials[20];
uniform unsigned int uMaterialId;

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void main()
{
	outFragPosition = vec4(vsPos, 1.0);
	outFragNormal = vec4(vsNormal, 1.0);
	outFragEmissive = vec4(uMaterials[uMaterialId].emissive, 1.0);
	outFragMaterialId = uMaterialId;
}