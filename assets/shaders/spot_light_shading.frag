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
uniform sampler2D uEmissiveTexture;
uniform usampler2D uMaterialIdTexture;

uniform Material uMaterials[20];

// wip light uniforms
uniform SpotLight uSpotLight;
uniform sampler2DShadow uShadowMap;
//uniform sampler2DShadow uShadowMap2;

uniform float uLightShaftExposure = 0.5;
uniform float uLightShaftRange = 2.8;
uniform int uLightShaftSamples = 40;

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

float sampleShadowMap(sampler2DShadow shadowMap, vec3 vsSamplePos)
{
	float shadow = 0.0;
	vec4 smCoord = uSpotLight.lightMatrix * vec4(vsSamplePos, 1.0);
	if (smCoord.z > 0.0) shadow = textureProj(shadowMap, smCoord);
	return shadow;
}

float calcLightScale(vec3 samplePos)
{
	float lightDist = length(uSpotLight.vsPos - samplePos);
	float rangeSquared = uSpotLight.range * uSpotLight.range;
	float lightDistSquared = lightDist * lightDist;
	return max((-1.0 / rangeSquared) * (lightDistSquared - rangeSquared), 0);
}

float lightShaftFactor(sampler2DShadow shadowMap, vec3 vsPos, int numSamples, float maxSampleDist,
                       vec3 vsLightPos)
{
	vec3 camDir = normalize(vsPos);
	float sampleLength = min(length(vsPos), maxSampleDist) / float(numSamples+1);
	vec3 toNextSamplePos = camDir * sampleLength;

	vec3 currentSamplePos = toNextSamplePos;
	float factor = 0.0;
	for (int i = 0; i < numSamples; i++) {
		factor += sampleShadowMap(shadowMap, currentSamplePos) * calcLightScale(currentSamplePos);
		currentSamplePos += toNextSamplePos;
	}
	factor /= float(numSamples);
	
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

	// Vectors
	vec3 toCam = normalize(-vsPos);
	vec3 toLight = normalize(uSpotLight.vsPos - vsPos);
	vec3 halfVec = normalize(toLight + toCam);

	// Diffuse lighting
	float diffuseIntensity = clamp(dot(toLight, vsNormal), 0.0, 1.0);
	vec3 diffuseContribution = diffuseIntensity * mtl.diffuse * uSpotLight.color;

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
	vec3 specularContribution = specularIntensity * materialSpecular * uSpotLight.color;

	// Shadow
	float shadow = sampleShadowMap(uShadowMap, vsPos);// * 0.5
	//             + sampleShadowMap(uShadowMap2, vsPos) * 0.5;

	// Spotlight scaling
	float lightScale = calcLightScale(vsPos);

	float lightShafts = lightShaftFactor(uShadowMap, vsPos, uLightShaftSamples, uLightShaftRange, uSpotLight.vsPos);


	// Total shading and output
	vec3 shading = diffuseContribution * shadow * lightScale
	             + specularContribution * shadow * lightScale
	             + uLightShaftExposure * lightShafts * uSpotLight.color;

	outFragColor = vec4(shading, 1.0);
}