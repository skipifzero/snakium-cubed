#version 330

// Structs
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct SpotLight {
	vec3 vsPos;
	vec3 vsDir;
	vec3 color;
	float range;
	float fovRad;
	float softAngleRad;
	mat4 lightMatrix;
};

// Input, output and uniforms
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Input
in vec2 uvCoord;

// Output
out vec4 outFragColor;

// Uniforms
uniform sampler2D uPositionTexture;
uniform sampler2D uLightShaftsTexture;
uniform SpotLight uSpotlight;
uniform sampler2DShadow uShadowMapLowRes;

const float shadowSampleWeight = 1.0;
const float lightSampleWeight = 1.0 / shadowSampleWeight;
uniform float uLightShaftExposure = 0.8;
uniform float uLightShaftRange = 5.0;
uniform int uLightShaftSamples = 128;

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

float sampleShadowMap(sampler2DShadow shadowMap, vec3 vsSamplePos)
{
	return textureProj(shadowMap, uSpotlight.lightMatrix * vec4(vsSamplePos, 1.0));
}

float calcOuterCos()
{
	return cos(uSpotlight.fovRad/2.0);
}

float calcInnerCos()
{
	return cos((uSpotlight.fovRad/2.0) - uSpotlight.softAngleRad);
}

float calcLightScale(vec3 samplePos, float outerCos, float innerCos)
{
	vec3 toSample = samplePos - uSpotlight.vsPos;
	vec3 toSampleDir = normalize(toSample);
	float lightDist = length(toSample);
	float rangeSquared = uSpotlight.range * uSpotlight.range;
	float lightDistSquared = lightDist * lightDist;
	float attenuation = smoothstep(outerCos, innerCos, dot(toSampleDir, uSpotlight.vsDir));
	return attenuation * max((-1.0 / rangeSquared) * (lightDistSquared - rangeSquared), 0);
}

float lightShaftFactor(sampler2DShadow shadowMap, vec3 vsPos, float outerCos, float innerCos)
{
	vec3 camDir = normalize(vsPos);
	float sampleLength = min(length(vsPos), uLightShaftRange) / float(uLightShaftSamples+1);
	vec3 toNextSamplePos = camDir * sampleLength;

	vec3 currentSamplePos = toNextSamplePos;
	float factor = 0.0;
	for (int i = 0; i < uLightShaftSamples; i++) {
		factor += sampleShadowMap(shadowMap, currentSamplePos) * calcLightScale(currentSamplePos, outerCos, innerCos);
		currentSamplePos += toNextSamplePos;
	}

	factor = (factor * lightSampleWeight) / float(uLightShaftSamples);
	factor *= shadowSampleWeight;
	
	return factor;
}

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void main()
{
	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 previousValue = texture(uLightShaftsTexture, uvCoord).xyz;

	float outerCos = calcOuterCos();
	float innerCos = calcInnerCos();
	float lightShafts = lightShaftFactor(uShadowMapLowRes, vsPos, outerCos, innerCos);

	vec3 total = uLightShaftExposure * lightShafts * uSpotlight.color;

	outFragColor = vec4(previousValue + total, 1.0);
}