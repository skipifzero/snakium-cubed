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
uniform sampler2DShadow uShadowMap;
uniform Spotlight uSpotlight;

uniform int uNumSamples = 128;
uniform float uMaxDist = 5.0;
uniform float uScaleFactor = 1.0;

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

// Intersection test
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct Intersection {
	bool hit;
	float t1, t2;
};

// length(lineDir) == 1
Intersection lineVsSphere(vec3 linePos, vec3 lineDir, vec3 spherePos, float sphereRadius)
{
	// ax² + bx + c = 0
	// x = (-b [+-] sqrt(b¹ - 4ac)) / (2a)
	// a = 1
	vec3 sphereToLine = linePos - spherePos;
	float b = 2 * dot(lineDir, sphereToLine);
	float c = dot(sphereToLine, sphereToLine) - sphereRadius * sphereRadius;

	// sqrt(b¹ - 4ac)
	// if inner factor is below zero there is no solution, i.e. no hit
	float sqrtInner = b * b - 4 * c;
	if (sqrtInner < 0) {
		return Intersection(false, 0, 0);
	}

	// Calculate intersections
	float sqrtProd = sqrt(sqrtInner);
	float t1 = (-b - sqrtProd) / 2;
	float t2 = (-b + sqrtProd) / 2;

	return Intersection(true, t1, t2);	
}

// length(coneDir) == 1
// length(rayDir) == 1
// cosConeAngle == cos(coneAngle) == cos(coneFov / 2)
Intersection lineVsInfiniteMirroredCone(vec3 linePos, vec3 lineDir, vec3 conePos, vec3 coneDir, float coneCosAngle)
{
	// Simplifications
	float cos2 = coneCosAngle * coneCosAngle;
	float dirsDot = dot(coneDir, lineDir);
	vec3 lineToCone = linePos - conePos;
	float coneDirDotLineToCone = dot(coneDir, lineToCone);

	// Calculations
	// ax² + bx + c = 0
	// x = (-b [+-] sqrt(b¹ - 4ac)) / (2a)
	float a = (dirsDot * dirsDot) - cos2;
	float b = 2.0 * (coneDirDotLineToCone * dirsDot - cos2 * dot(lineToCone, lineDir));
	float c = coneDirDotLineToCone * coneDirDotLineToCone - cos2 * dot(lineToCone, lineToCone);

	// sqrt(b¹ - 4ac)
	// if inner factor is below zero there is no solution, i.e. no hit
	float sqrtInner = b * b - 4 * a * c;
	if (sqrtInner < 0) {
		return Intersection(false, 0.0, 0.0);
	}

	// Calculate closest and farthest points
	float sqrtProd = sqrt(sqrtInner);
	float a2 = 2 * a;
	float t1 = (-b - sqrtProd) / a2;
	float t2 = (-b + sqrtProd) / a2;

	return Intersection(true, t1, t2);
}

// length(coneDir) == 1
// length(rayDir) == 1
// cosConeAngle == cos(coneAngle) == cos(coneFov / 2)
Intersection rayVsFiniteCone(vec3 rayPos, vec3 rayDir, vec3 conePos, vec3 coneDir, float coneCosAngle, float coneRange)
{
	Intersection coneIsect = lineVsInfiniteMirroredCone(rayPos, rayDir, conePos, coneDir, coneCosAngle);

	// Early rejection test, if infinite mirrored cone was not hit there is no point in continuing
	if (!coneIsect.hit) {
		return Intersection(false, 0, 0);
	}

	// Calculate intersection points for infinite mirrored cone and sphere
	Intersection sphereIsect = lineVsSphere(rayPos, rayDir, conePos, coneRange);
	vec3 c1 = rayPos + rayDir * coneIsect.t1;
	vec3 c2 = rayPos + rayDir * coneIsect.t2;
	vec3 s1 = rayPos + rayDir * sphereIsect.t1;
	vec3 s2 = rayPos + rayDir * sphereIsect.t2;
	
	vec3 coneToC1 = c1 - conePos;
	vec3 coneToC2 = c2 - conePos;
	vec3 coneToS1 = s1 - conePos;
	vec3 coneToS2 = s2 - conePos;

	// Try to find 2 points which intersects the real cone 
	float hits[4]; // Needs 4 for literal edge cases.
	int index = 0;

	if (dot(coneToC1, coneDir) > 0 && length(coneToC1) < coneRange) {
		hits[index++] = coneIsect.t1;
	}
	if (dot(coneToC2, coneDir) > 0 && length(coneToC2) < coneRange) {
		hits[index++] = coneIsect.t2;	
	}
	if (dot(normalize(coneToS1), coneDir) > coneCosAngle) {
		hits[index++] = sphereIsect.t1;
	}
	if (dot(normalize(coneToS2), coneDir) > coneCosAngle) {
		hits[index++] = sphereIsect.t2;
	}

	// Sort the 2 found intersection points
	float closest = min(hits[0], hits[1]);
	float farthest = max(hits[0], hits[1]);

	// if (index != 2) return Intersection(false, 0, 0);
	//
	// Case where both intersections are behind ray
	// if (farthest < 0) return Intersection(false, 0, 0);
	//
	// Case where closest is behind ray (can only happen when ray starts inside finite cone)
	// else if (closest < 0) return Intersection(true, 0, farthest);
	//
	// Case where both intersections are ahead of ray
	// else return Intersection(true, closest, farthest);
	return Intersection(index == 2 && farthest >= 0, max(0, closest), max(0, farthest));
}

// Main
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

//#define BASELINE
//#define NAIVE_MARCHING
//#define INTERSECTION_TEST_NO_SAMPLING
//#define INTERSECTION_TEST_NAIVE_MARCHING
//#define INTERSECTION_TEST_OPTIMIZED_FIXED_SAMPLING
#define INTERSECTION_TEST_OPTIMIZED_DYNAMIC_SAMPLING

void main()
{
#ifdef BASELINE
	outFragColor = vec4(0);
	return;
#endif

	// Ray information
	float linDepth = texture(uLinearDepthTexture, uvCoord).r;
	vec3 vsPos = uFarPlaneDist * linDepth * nonNormRayDir / abs(nonNormRayDir.z);
	float distToPos = length(vsPos);
	vec3 rayDir = normalize(nonNormRayDir);

	// Eye distance weight function
	// f(x) = m + k * x
	// m = 2 / uMaxDist, k = -2 / uMaxDist²
	// f(uMaxDist) = 0, integral f(x) under [0, uMaxDist] = 1
	float eyeWeightM = 2.0 / uMaxDist;
	float eyeWeightK = -2.0 / (uMaxDist * uMaxDist);

#ifdef NAIVE_MARCHING
	float sampleStep = (min(distToPos, uMaxDist) / float(uNumSamples - 1));
	float factor = 0.0;
	for (int i = 0; i < uNumSamples; ++i) {
		float sampleT = float(i) * sampleStep;
		vec3 samplePos = sampleT * rayDir;

		float shadowSample = sampleShadowMap(samplePos);
		float dissipation = calcLightDissipation(samplePos);
		float attenuation = calcLightAttenuation(samplePos);
		float eyeDistWeight = eyeWeightM + eyeWeightK * sampleT;

		factor += shadowSample * dissipation * attenuation * eyeDistWeight * sampleStep;
	}

	outFragColor = vec4(uScaleFactor * factor * uSpotlight.color, 1.0);
	return;
#endif

	// Ray vs cone intersection test
	Intersection isect = rayVsFiniteCone(vec3(0), rayDir, uSpotlight.vsPos, uSpotlight.vsDir, uSpotlight.softAngleCos, uSpotlight.range);
	if (!isect.hit) {
		discard;
		return;
	}
	float endT = min(min(isect.t2, distToPos), uMaxDist);
	float startT = min(isect.t1, endT);
	if (endT == startT) {
		discard;
		return;
	}
	vec3 startPos = rayDir * startT;
	vec3 endPos = rayDir * endT;

#ifdef INTERSECTION_TEST_NO_SAMPLING
	float sampleStep = (endT - startT) / 15.0;
	float factor = 0.0;
	for (int i = 0; i < 16; ++i) {
		float sampleT = startT + float(i) * sampleStep;
		vec3 samplePos = sampleT * rayDir;

		float dissipation = calcLightDissipation(samplePos);
		float attenuation = calcLightAttenuation(samplePos);
		float eyeDistWeight = eyeWeightM + eyeWeightK * sampleT;

		factor += dissipation * attenuation * eyeDistWeight * sampleStep;
	}
#endif

#ifdef INTERSECTION_TEST_NAIVE_MARCHING
	float sampleStep = (endT - startT) / float(uNumSamples - 1);
	float factor = 0.0;
	for (int i = 0; i < uNumSamples; ++i) {
		float sampleT = startT + float(i) * sampleStep;
		vec3 samplePos = sampleT * rayDir;

		float shadowSample = sampleShadowMap(samplePos);
		float dissipation = calcLightDissipation(samplePos);
		float attenuation = calcLightAttenuation(samplePos);
		float eyeDistWeight = eyeWeightM + eyeWeightK * sampleT;

		factor += shadowSample * dissipation * attenuation * eyeDistWeight * sampleStep;
	}
#endif

#ifdef INTERSECTION_TEST_OPTIMIZED_FIXED_SAMPLING
	float sampleStep = (endT - startT) / float(uNumSamples - 1);
	float interpStep = 1.0 / float(uNumSamples - 1);

	// Precompute shadow coord
	vec4 startShadowCoord = uSpotlight.lightMatrix * vec4(startPos, 1.0);
	vec4 endShadowCoord = uSpotlight.lightMatrix * vec4(endPos, 1.0);

	// Precompute light dissipation variables
	vec3 startLightToSample = startPos - uSpotlight.vsPos;
	vec3 endLightToSample = endPos - uSpotlight.vsPos;
	float invSquaredLightRange = 1.0 / (uSpotlight.range * uSpotlight.range);

	// Precompute eye dist weight and monte carlo weight and combine
	float startWeight = (eyeWeightM + eyeWeightK * startT) * sampleStep;
	float endWeight = (eyeWeightM + eyeWeightK * endT) * sampleStep;

	float factor = 0.0;
	for (int i = 0; i < uNumSamples; ++i) {

		float interp = interpStep * float(i);

		vec4 sampleShadowCoord = mix(startShadowCoord, endShadowCoord, interp);
		float shadowSample = textureProj(uShadowMap, sampleShadowCoord);

		vec3 lightToSample = mix(startLightToSample, endLightToSample, interp);
		float dissipation = 1.0 - dot(lightToSample, lightToSample) * invSquaredLightRange;

		vec3 lightToSampleDir = normalize(lightToSample);
		float attenuation = smoothstep(uSpotlight.softAngleCos, uSpotlight.sharpAngleCos, dot(lightToSampleDir, uSpotlight.vsDir));

		float weight = mix(startWeight, endWeight, interp);

		factor += shadowSample * dissipation * attenuation * weight;
	}
#endif

#ifdef INTERSECTION_TEST_OPTIMIZED_DYNAMIC_SAMPLING

	// Precompute shadow coord
	vec4 startShadowCoord = uSpotlight.lightMatrix * vec4(startPos, 1.0);
	vec4 endShadowCoord = uSpotlight.lightMatrix * vec4(endPos, 1.0);

	// Calculate how many samples to take
	vec2 shadowMapSize = vec2(textureSize(uShadowMap, 0));
	vec2 diff = abs((endShadowCoord.xy / endShadowCoord.w) - (startShadowCoord.xy / startShadowCoord.w));
	vec2 texelDiff = diff * shadowMapSize;
	int numSamples = clamp(int(max(texelDiff.x, texelDiff.y)), 16, uNumSamples);

	float sampleStep = (endT - startT) / float(numSamples - 1);
	float interpStep = 1.0 / float(numSamples - 1);

	// Precompute light dissipation variables
	vec3 startLightToSample = startPos - uSpotlight.vsPos;
	vec3 endLightToSample = endPos - uSpotlight.vsPos;
	float invSquaredLightRange = 1.0 / (uSpotlight.range * uSpotlight.range);

	// Precompute eye dist weight and monte carlo weight and combine
	float startWeight = (eyeWeightM + eyeWeightK * startT) * sampleStep;
	float endWeight = (eyeWeightM + eyeWeightK * endT) * sampleStep;

	float factor = 0.0;
	for (int i = 0; i < numSamples; ++i) {

		float interp = interpStep * float(i);

		vec4 sampleShadowCoord = mix(startShadowCoord, endShadowCoord, interp);
		float shadowSample = textureProj(uShadowMap, sampleShadowCoord);

		vec3 lightToSample = mix(startLightToSample, endLightToSample, interp);
		float dissipation = 1.0 - dot(lightToSample, lightToSample) * invSquaredLightRange;

		vec3 lightToSampleDir = normalize(lightToSample);
		float attenuation = smoothstep(uSpotlight.softAngleCos, uSpotlight.sharpAngleCos, dot(lightToSampleDir, uSpotlight.vsDir));

		float weight = mix(startWeight, endWeight, interp);

		factor += shadowSample * dissipation * attenuation * weight;
	}
#endif

#ifndef BASELINE
	outFragColor = vec4(uScaleFactor * factor * uSpotlight.color, 1.0);
#endif
}
