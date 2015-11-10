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
in vec3 vsPos;
in vec3 vsNormal;

// Output
out vec4 outFragColor;

// Uniforms
uniform Material uMaterial;

// wip light uniforms
uniform SpotLight uSpotLight;
uniform sampler2DShadow uShadowMap;
uniform sampler2DShadow uShadowMap2;

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
	// Vectors
	vec3 toCam = normalize(-vsPos);
	vec3 toLight = normalize(uSpotLight.vsPos - vsPos);
	vec3 halfVec = normalize(toLight + toCam);

	// Ambient lighting
	vec3 ambientContribution = uMaterial.ambient * uSpotLight.color;

	// Diffuse lighting
	float diffuseIntensity = clamp(dot(toLight, vsNormal), 0.0, 1.0);
	vec3 diffuseContribution = diffuseIntensity * uMaterial.diffuse * uSpotLight.color;

	// Fresnel effect
	vec3 materialSpecular = uMaterial.specular;
	float fresnelBase = clamp(1.0 - clamp(dot(vsNormal, toCam), 0.0, 1.0), 0.0, 1.0);
	float fresnel = pow(fresnelBase, 5.0);
	materialSpecular = materialSpecular + (vec3(1.0) - materialSpecular) * fresnel;

	// Specular lighting
	float specularAngle = 0.0;
	if (diffuseIntensity > 0.0) {
		specularAngle = clamp(dot(vsNormal, halfVec), 0.0, 1.0);
	}
	float specularIntensity = pow(specularAngle, uMaterial.shininess);
	///specularIntensity *= ((uMaterial.shininess + 2.0) / 8.0); // Normalization
	vec3 specularContribution = specularIntensity * materialSpecular * uSpotLight.color;

	// Shadow
	float shadow = sampleShadowMap(uShadowMap, vsPos);// * 0.5
	//             + sampleShadowMap(uShadowMap2, vsPos) * 0.5;

	// Spotlight scaling
	float lightScale = calcQuadraticLightScale(vsPos);

	// Total shading and output
	vec3 shading = ambientContribution
	             + diffuseContribution * shadow * lightScale
	             + specularContribution * shadow * lightScale
	             + uMaterial.emissive;

	outFragColor = vec4(shading, uMaterial.opaque);
}