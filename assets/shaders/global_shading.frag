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
in vec2 uvCoord;

// Output
out vec4 outFragColor;

// Uniforms
uniform sampler2D uPositionTexture;
uniform sampler2D uNormalTexture;
uniform usampler2D uMaterialIdTexture;
uniform sampler2D uSpotlightShadingTexture;
uniform sampler2D uBlurredEmissiveTexture;

uniform Material uMaterials[20];

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void main()
{
	vec3 AMBIENT_LIGHT = vec3(1);

	// Values from GBuffer
	//vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	//vec3 vsNormal = texture(uNormalTexture, uvCoord).xyz;
	uint materialId = texture(uMaterialIdTexture, uvCoord).r;
	Material mtl = uMaterials[materialId];
	vec3 blurredEmissive = texture(uBlurredEmissiveTexture, uvCoord).rgb;

	vec3 spotlightShading = texture(uSpotlightShadingTexture, uvCoord).rgb;

	// Ambient lighting
	vec3 ambientContribution = mtl.ambient * AMBIENT_LIGHT;

	// Total shading and output
	vec3 shading = vec3(0);
	shading +=ambientContribution;
	shading += spotlightShading;
	shading += (float(mtl.emissive == vec3(0)) * blurredEmissive * 1.1);
	shading += mtl.emissive;

	outFragColor = vec4(shading, 1.0);
}