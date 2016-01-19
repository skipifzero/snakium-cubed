#version 330

// Structs
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct Spotlight {
	vec3 vsPos;
	vec3 vsDir;
	vec3 color;
	float range;
	float softFovRad; // outer
	float sharpFovRad; //inner
	float softAngleCos; // outer
	float sharpAngleCos; // inner
	mat4 lightMatrix;
};

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

uniform Material uMaterials[20];

uniform Spotlight uSpotlight;
uniform sampler2DShadow uShadowMap;

// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

float sampleShadowMap(vec3 vsSamplePos)
{
	return textureProj(uShadowMap, uSpotlight.lightMatrix * vec4(vsSamplePos, 1.0));
}

float calcLightDissipation(vec3 samplePos)
{
	vec3 lightToSample = samplePos - uSpotlight.vsPos;

	// Linear dissipation
	// f(x) = 1 - (x / range)
	// f(0) = 1, f(range) = 0
	//return clamp(1.0 - (length(lightToSample) / uSpotlight.range), 0.0, 1.0);

	// Quadratic dissipation
	// f(x) = 1 - (x² / range²)
	// f(0) = 1, f(range) = 0
	return clamp(1.0 - (dot(lightToSample, lightToSample) / (uSpotlight.range * uSpotlight.range)), 0.0, 1.0);
}

float calcLightAttenuation(vec3 samplePos)
{
	vec3 lightToSampleDir = normalize(samplePos - uSpotlight.vsPos);
	return smoothstep(uSpotlight.softAngleCos, uSpotlight.sharpAngleCos, dot(lightToSampleDir, uSpotlight.vsDir));
}


// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void main()
{
	// Values from GBuffer
	float linDepth = texture(uLinearDepthTexture, uvCoord).r;
	vec3 vsPos = uFarPlaneDist * linDepth * nonNormRayDir / abs(nonNormRayDir.z);
	vec3 vsNormal = texture(uNormalTexture, uvCoord).xyz;
	uint materialId = texture(uMaterialIdTexture, uvCoord).r;
	Material mtl = uMaterials[materialId];

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

	// Shadow, dissipation & attenuation
	float shadow = sampleShadowMap(vsPos);
	float dissipation = calcLightDissipation(vsPos);
	float attenuation = calcLightAttenuation(vsPos);

	// Total shading and output
	vec3 shading = shadow * dissipation * attenuation * (diffuseContribution + specularContribution);
	
	outFragColor = vec4(shading, 1.0);
}