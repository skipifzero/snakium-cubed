#version 330

precision highp float; // required by GLSL spec Sect 4.5.3

// Input
in vec3 vsPos;
in vec3 vsNormal;

// Output
out vec4 outFragColor;

// Uniforms
uniform vec4 uColor;

// wip light uniforms
uniform vec3 uSpotLightPos;
uniform vec3 uSpotLightDir;
uniform float uSpotLightReach;
uniform float uSpotLightAngle;
uniform mat4 uLightMatrix;
uniform sampler2DShadow uShadowMap;

// Constants
const float materialShininess = 8.0;
const vec3 ambientLight = vec3(0.2);

float sampleShadowMap(vec3 vsSamplePos)
{
	float shadow = 0.0;
	vec4 smCoord = uLightMatrix * vec4(vsSamplePos, 1.0);
	if (smCoord.z > 0.0) shadow = textureProj(uShadowMap, smCoord);
	return shadow;
}

void main()
{
	// Materials
	vec3 materialAmbient = uColor.rgb;
	vec3 materialDiffuse = uColor.rgb;
	vec3 materialSpecular = uColor.rgb;
	vec3 materialEmissive = vec3(0.0);

	// Vectors
	vec3 toCam = normalize(-vsPos);
	vec3 toLight = normalize(uSpotLightPos - vsPos);
	vec3 halfVec = normalize(toLight + toCam);

	// Ambient lighting
	vec3 ambientContribution = ambientLight * materialAmbient;

	// Diffuse lighting
	float diffuseIntensity = clamp(dot(toLight, vsNormal), 0.0, 1.0);
	vec3 diffuseContribution = diffuseIntensity * materialDiffuse;

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
	//specularIntensity *= ((materialShininess + 2.0) / 8.0); // Normalization
	vec3 specularContribution = specularIntensity * materialSpecular;

	// Shadow
	float shadow = sampleShadowMap(vsPos);

	// Total shading and output
	vec3 shading = ambientContribution
	             + diffuseContribution * shadow
	             + specularContribution * shadow
	             + materialEmissive;

	outFragColor = vec4(shading, uColor.a);
}