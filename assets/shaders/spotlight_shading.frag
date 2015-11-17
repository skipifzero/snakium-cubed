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
uniform sampler2DShadow uShadowMapHighRes;
uniform sampler2DShadow uShadowMapLowRes;

const float shadowSampleWeight = 1.0;
const float lightSampleWeight = 1.0 / shadowSampleWeight;
uniform float uLightShaftExposure = 0.7;
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

	//float angle = dot(posToLight, -viewSpaceLightDir);
	//float spotAttenuation = (angle > spotOpeningAngle) ? 1.0 : 0.0;
	//float spotAttenuation = smoothstep(spotOuterAngle, spotInnerAngle, angle);

	///vec3 toSample = normalize(samplePos - uSpotlight.vsPos);
	/*float attenuation = smoothstep(uSpotlight.fovRad, uSpotlight.fovRad - uSpotlight.softAngleRad,
	                    dot(toSample, uSpotlight.vsDir));*/
	//float attenuation = ((dot(toSample, uSpotlight.vsDir) > cos(0.3)) ? 1.0 : 0.0);
	//float attenuation = dot(toSample, uSpotlight.vsPos);//smoothstep(0.3, 0.5, dot(toSample, uSpotlight.vsDir));
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
	float shadow = sampleShadowMap(uShadowMapHighRes, vsPos);

	// Spotlight scaling
	float outerCos = calcOuterCos();
	float innerCos = calcInnerCos();
	float lightScale = calcLightScale(vsPos, outerCos, innerCos);

	float lightShafts = lightShaftFactor(uShadowMapLowRes, vsPos, outerCos, innerCos);


	// Total shading and output
	vec3 shading = diffuseContribution * shadow * lightScale
	             + specularContribution * shadow * lightScale
	             + uLightShaftExposure * lightShafts * vec3(0.2, 1.0, 0.6);//uSpotlight.color;


	outFragColor = vec4(previousSpotlightValue + shading, 1.0);

	//outFragColor = vec4(vec3(uSpotlight.vsPos), 1.0);
}