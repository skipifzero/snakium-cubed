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

// Input, output and uniforms
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Input
in vec3 vsPos;
in vec3 vsNormal;

// Output
out vec4 outFragColor;

// Uniforms
uniform vec4 uColor;

// wip light uniforms
uniform SpotLight uSpotLight;
uniform sampler2DShadow uShadowMap;
uniform sampler2DShadow uShadowMap2;

// Constants
const float materialShininess = 8.0;

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

float sampleShadowMap(sampler2DShadow shadowMap, vec3 vsSamplePos)
{
	float shadow = 0.0;
	vec4 smCoord = uSpotLight.lightMatrix * vec4(vsSamplePos, 1.0);
	if (smCoord.z > 0.0) shadow = textureProj(shadowMap, smCoord);
	return shadow;
}

float calcQuadraticLightScale(vec3 samplePos)
{
	float lightDist = length(uSpotLight.vsPos - samplePos);
	float rangeSquared = uSpotLight.range * uSpotLight.range;
	float lightDistSquared = lightDist * lightDist;
	return max((-1.0 / rangeSquared) * (lightDistSquared - rangeSquared), 0);
}

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void main()
{
	// Materials
	vec3 materialAmbient = uColor.rgb * 0.1;
	vec3 materialDiffuse = uColor.rgb * 0.5;
	vec3 materialSpecular = uColor.rgb * 0.75;
	vec3 materialEmissive = vec3(0.0);

	// Vectors
	vec3 toCam = normalize(-vsPos);
	vec3 toLight = normalize(uSpotLight.vsPos - vsPos);
	vec3 halfVec = normalize(toLight + toCam);

	// Ambient lighting
	vec3 ambientContribution = materialAmbient * uSpotLight.color;

	// Diffuse lighting
	float diffuseIntensity = clamp(dot(toLight, vsNormal), 0.0, 1.0);
	vec3 diffuseContribution = diffuseIntensity * materialDiffuse * uSpotLight.color;

	// Fresnel effect
	float fresnelBase = clamp(1.0 - clamp(dot(vsNormal, toCam), 0.0, 1.0), 0.0, 1.0);
	float fresnel = pow(fresnelBase, 5.0);
	materialSpecular = materialSpecular + (vec3(1.0) - materialSpecular) * fresnel;

	// Specular lighting
	float specularAngle = 0.0;
	if (diffuseIntensity > 0.0) {
		specularAngle = clamp(dot(vsNormal, halfVec), 0.0, 1.0);
	}
	float specularIntensity = pow(specularAngle, materialShininess);
	specularIntensity *= ((materialShininess + 2.0) / 8.0); // Normalization
	vec3 specularContribution = specularIntensity * materialSpecular * uSpotLight.color;

	// Shadow
	float shadow = sampleShadowMap(uShadowMap, vsPos) * 0.5
	             + sampleShadowMap(uShadowMap2, vsPos) * 0.5;

	// Spotlight scaling
	float lightScale = calcQuadraticLightScale(vsPos);

	// Total shading and output
	vec3 shading = ambientContribution
	             + diffuseContribution * shadow * lightScale
	             + specularContribution * shadow * lightScale
	             + materialEmissive;

	outFragColor = vec4(shading, uColor.a);
}