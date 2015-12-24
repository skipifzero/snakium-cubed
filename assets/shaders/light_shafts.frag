#version 330

// Structs
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct SpotLight {
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

// Output
out vec4 outFragColor;

// Uniforms
uniform sampler2D uPositionTexture;
uniform SpotLight uSpotlight;
uniform sampler2DShadow uShadowMapLowRes;


uniform float uLightShaftMaxRange = 10;
uniform float uLightShaftFullVisibilityRange = 1.5;
uniform int uLightShaftMaxNumSamples = 128;


// Helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

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

void main()
{
	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;

	// Ray vs Cone Intersection test parameters
	vec3 camPos = vec3(0);
	vec3 camDir = normalize(vsPos);
	vec3 lightPos = uSpotlight.vsPos;
	vec3 lightDir = uSpotlight.vsDir;
	float lightCosAngle = uSpotlight.softAngleCos;
	float lightRange = uSpotlight.range;

	// Ray vs Cone intersection test
	Intersection isect = rayVsFiniteCone(camPos, camDir, lightPos, lightDir, lightCosAngle, lightRange);
	if (!isect.hit) {
		discard;
	}

	// Calculate closest and farthest points on the light shaft
	float closestDist = isect.t1;
	float farthestDist = min(min(isect.t2, length(vsPos)), uLightShaftMaxRange);
	vec3 closest = camPos + camDir * closestDist;
	vec3 farthest = camPos + camDir * farthestDist;

//#define SAMPLE_IN_CLIP_SPACE
#ifdef SAMPLE_IN_CLIP_SPACE

	vec4 closestClipSpace = uSpotlight.lightMatrix * vec4(closest, 1.0);
	vec4 farthestClipSpace = uSpotlight.lightMatrix * vec4(farthest, 1.0);

	vec4 sampleVector = farthestClipSpace - closestClipSpace;
	vec4 sampleDir = normalize(sampleVector);
	float sampleDist = length(sampleVector);
	float sampleStepSize = sampleDist / float(uLightShaftMaxNumSamples-1); // 2 samples -> step = 1.0 so we sample both edges of light shaft
	vec4 sampleStep = sampleDir * sampleStepSize;

	float factor = 0.0;
	for (int i = 0; i < uLightShaftMaxNumSamples; ++i) {
		vec4 samplePos = closestClipSpace + float(i) * sampleStep;
		factor += textureProj(uShadowMapLowRes, samplePos);
		// float scale = calcLightScale(currentSamplePos, uSpotlight.softAngleCos, uSpotlight.sharpAngleCos);
		// ffactor += sample * scale;
	}
	factor /= float(uLightShaftMaxNumSamples);

#else

	vec3 sampleVector = farthest - closest;
	vec3 sampleStep = (length(sampleVector) / float(uLightShaftMaxNumSamples-1)) * normalize(sampleVector);
	float factor = 0.0;
	for (int i = 0; i < uLightShaftMaxNumSamples; ++i) {
		vec3 samplePos = closest + float(i) * sampleStep;
		float sample = textureProj(uShadowMapLowRes, uSpotlight.lightMatrix * vec4(samplePos, 1));
		float scale = calcLightScale(samplePos, uSpotlight.softAngleCos, uSpotlight.sharpAngleCos);
		factor += sample * scale;
	}
	factor /= float(uLightShaftMaxNumSamples);

#endif

	float weight = ((farthestDist - closestDist) / uLightShaftFullVisibilityRange) * factor;
	outFragColor = vec4(weight * uSpotlight.color, 1.0);
}