#version 330

// Structs
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct Material {
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
in vec3 nonNormRayDir;

// Output
out vec4 outFragColor;

// Uniforms
uniform float uFarPlaneDist;
uniform sampler2D uLinearDepthTexture;
uniform sampler2D uNormalTexture;
uniform usampler2D uMaterialIdTexture;
uniform sampler2D uTransparencyTexture;
uniform sampler2D uSpotlightShadingTexture;
//uniform sampler2D uLightShaftsTexture;
uniform sampler2D uBlurredEmissiveTexture;

uniform Material uMaterials[20];
uniform vec3 uAmbientLight;

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void main()
{
	// Values from GBuffer
	//float linDepth = texture(uLinearDepthTexture, uvCoord).r;
	//vec3 vsPos = uFarPlaneDist * linDepth * nonNormRayDir / abs(nonNormRayDir.z);
	//vec3 vsNormal = texture(uNormalTexture, uvCoord).xyz;
	uint materialId = texture(uMaterialIdTexture, uvCoord).r;
	Material mtl = uMaterials[materialId];

	vec4 transparency = texture(uTransparencyTexture, uvCoord);
	vec3 spotlightShading = texture(uSpotlightShadingTexture, uvCoord).rgb;
	//vec3 lightShafts = texture(uLightShaftsTexture, uvCoord).rgb;
	vec3 blurredEmissive = texture(uBlurredEmissiveTexture, uvCoord).rgb;

	// Ambient lighting
	vec3 ambientContribution = mtl.diffuse * uAmbientLight;

	// Total shading and output
	vec3 shading = vec3(0);
	shading += ambientContribution;
	shading += spotlightShading;
	shading += (float(mtl.emissive == vec3(0)) * blurredEmissive * 2.5);
	shading += mtl.emissive;

	shading = transparency.rgb * transparency.a + (1 - transparency.a) * shading;
	//shading += lightShafts;

	outFragColor = vec4(shading, 1.0);
}