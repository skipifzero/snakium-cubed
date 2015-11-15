#version 330

// Structs
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct SpotLight {
	vec3 vsPos;
	vec3 vsDir;
	vec3 color;
	float range;
	float fov;
	mat4 lightMatrix;
};

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
uniform sampler2D uSpotlightTexture;

uniform Material uMaterials[20];

uniform SpotLight uSpotlight;
uniform sampler2DShadow uShadowMap;
//uniform sampler2DShadow uShadowMap2;

uniform float uLightShaftExposure = 0.75;
uniform float uLightShaftRange = 3.0;
uniform int uLightShaftSamples = 64;

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

float sampleShadowMap(sampler2DShadow shadowMap, vec3 vsSamplePos)
{
	float shadow = 0.0;
	vec4 smCoord = uSpotlight.lightMatrix * vec4(vsSamplePos, 1.0);
	if (smCoord.z > 0.0) shadow = textureProj(shadowMap, smCoord);
	return shadow;
}

float calcLightScale(vec3 samplePos)
{
	float lightDist = length(uSpotlight.vsPos - samplePos);
	float rangeSquared = uSpotlight.range * uSpotlight.range;
	float lightDistSquared = lightDist * lightDist;
	return max((-1.0 / rangeSquared) * (lightDistSquared - rangeSquared), 0);
}

float lightShaftFactor(sampler2DShadow shadowMap, vec3 vsPos)
{
	vec3 camDir = normalize(vsPos);
	float sampleLength = min(length(vsPos), uLightShaftRange) / float(uLightShaftSamples+1);
	vec3 toNextSamplePos = camDir * sampleLength;

	vec3 currentSamplePos = toNextSamplePos;
	float factor = 0.0;
	for (int i = 0; i < uLightShaftSamples; i++) {
		factor += sampleShadowMap(shadowMap, currentSamplePos) * calcLightScale(currentSamplePos);
		currentSamplePos += toNextSamplePos;
	}
	factor /= float(uLightShaftSamples);
	
	return factor;
}

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void main()
{
	// Values from GBuffer
	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 vsNormal = texture(uNormalTexture, uvCoord).xyz;
	uint materialId = texture(uMaterialIdTexture, uvCoord).r;
	Material mtl = uMaterials[materialId];

	// Previous value
	vec3 previousSpotlightValue = texture(uSpotlightTexture, uvCoord).rgb;

	// Vectors
	vec3 toCam = normalize(-vsPos);
	vec3 toLight = normalize(uSpotlight.vsPos - vsPos);
	vec3 halfVec = normalize(toLight + toCam);

	// Diffuse lighting
	float diffuseIntensity = clamp(dot(toLight, vsNormal), 0.0, 1.0);
	vec3 diffuseContribution = diffuseIntensity * mtl.diffuse * uSpotlight.color;

	// Fresnel effect
	vec3 materialSpecular = mtl.specular;
	float fresnelBase = clamp(1.0 - clamp(dot(vsNormal, toCam), 0.0, 1.0), 0.0, 1.0);
	float fresnel = pow(fresnelBase, 5.0);
	materialSpecular = materialSpecular + (vec3(1.0) - materialSpecular) * fresnel;

	// Specular lighting
	float specularAngle = 0.0;
	if (diffuseIntensity > 0.0) {
		specularAngle = clamp(dot(vsNormal, halfVec), 0.0, 1.0);
	}
	float specularIntensity = pow(specularAngle, mtl.shininess);
	///specularIntensity *= ((mtl.shininess + 2.0) / 8.0); // Normalization
	vec3 specularContribution = specularIntensity * materialSpecular * uSpotlight.color;

	// Shadow
	float shadow = sampleShadowMap(uShadowMap, vsPos);// * 0.5
	//             + sampleShadowMap(uShadowMap2, vsPos) * 0.5;

	// Spotlight scaling
	float lightScale = calcLightScale(vsPos);

	float lightShafts = lightShaftFactor(uShadowMap, vsPos);


	// Total shading and output
	vec3 shading = diffuseContribution * shadow * lightScale
	             + specularContribution * shadow * lightScale
	             + uLightShaftExposure * lightShafts * uSpotlight.color;


	outFragColor = vec4(previousSpotlightValue + shading, 1.0);
}