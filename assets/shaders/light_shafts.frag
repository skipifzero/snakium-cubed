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
//#define NAIVE_MARCHING_WITH_LIGHT_SCALING
#define ONLY_INTERSECTION_TEST
//#define NAIVE_INTERSECTION_TEST_MARCHING
//#define MINIMAL_INTERSECTION_TEST_MARCHING
//#define OPTIMIZED_FIXED_SAMPLING
//#define OPTIMIZED_FIXED_SAMPLING_WITH_ATTENUATION
//#define OPTIMIZED_DYNAMIC_SAMPLING
//#define OPTIMIZED_DYNAMIC_SAMPLING_WITH_ATTENUATION

void main()
{

#ifdef BASELINE
	outFragColor = vec4(0);
#endif


#ifdef NAIVE_MARCHING
	const int NUM_SAMPLES = 128;
	const float MAX_DIST = 5.0;

	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 camDir = normalize(vsPos);
	vec3 sampleStep = (max(length(vsPos), MAX_DIST) / float(NUM_SAMPLES - 1)) * camDir;

	float factor = 0.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		vec3 samplePos = float(i) * sampleStep;
		float sample = textureProj(uShadowMapLowRes, uSpotlight.lightMatrix * vec4(samplePos, 1.0));
		factor += sample;
	}
	factor /= float(NUM_SAMPLES);

	outFragColor = vec4(factor * uSpotlight.color, 1.0);
#endif


#ifdef NAIVE_MARCHING_WITH_LIGHT_SCALING
	const int NUM_SAMPLES = 128;
	const float MAX_DIST = 5.0;

	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 camDir = normalize(vsPos);
	vec3 sampleStep = (max(length(vsPos), MAX_DIST) / float(NUM_SAMPLES - 1)) * camDir;

	float factor = 0.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		vec3 samplePos = float(i) * sampleStep;
		float sample = textureProj(uShadowMapLowRes, uSpotlight.lightMatrix * vec4(samplePos, 1.0));

		// Scale
		vec3 toSample = samplePos - uSpotlight.vsPos;
		vec3 toSampleDir = normalize(toSample);
		float lightDist = length(toSample);
		float rangeSquared = uSpotlight.range * uSpotlight.range;
		float lightDistSquared = lightDist * lightDist;
		float attenuation = smoothstep(uSpotlight.softAngleCos, uSpotlight.sharpAngleCos, dot(toSampleDir, uSpotlight.vsDir));
		float scale = attenuation * max((-1.0 / rangeSquared) * (lightDistSquared - rangeSquared), 0);

		factor += sample * scale;
	}
	factor /= float(NUM_SAMPLES);

	outFragColor = vec4(factor * uSpotlight.color, 1.0);
#endif


#ifdef ONLY_INTERSECTION_TEST
	const float FULL_VISIBILITY_DIST = 1.5;

	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 camDir = normalize(vsPos);

	// Ray vs Cone intersection test
	Intersection isect = rayVsFiniteCone(vec3(0), camDir, uSpotlight.vsPos, uSpotlight.vsDir, uSpotlight.softAngleCos, uSpotlight.range);
	if (!isect.hit) {
		discard;
	}

	float weight = (isect.t2 - isect.t1) / FULL_VISIBILITY_DIST;
	outFragColor = vec4(vec3(weight * uSpotlight.color), 1.0);
#endif


#ifdef NAIVE_INTERSECTION_TEST_MARCHING
	const int NUM_SAMPLES = 128;
	const float MAX_DIST = 5.0;
	const float FULL_VISIBILITY_DIST = 1.5;

	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 camDir = normalize(vsPos);

	// Ray vs Cone intersection test
	Intersection isect = rayVsFiniteCone(vec3(0), camDir, uSpotlight.vsPos, uSpotlight.vsDir, uSpotlight.softAngleCos, uSpotlight.range);
	if (!isect.hit) {
		discard;
	}

	float startT = isect.t1;
	float endT = min(min(isect.t2, length(vsPos)), MAX_DIST);
	vec3 start = camDir * startT;
	vec3 end = camDir * endT;
	vec3 sampleStep = (length(end - start) / float(NUM_SAMPLES - 1)) * camDir;

	float factor = 0.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		vec3 samplePos = start + float(i) * sampleStep;
		float sample = textureProj(uShadowMapLowRes, uSpotlight.lightMatrix * vec4(samplePos, 1.0));

		// Scale
		vec3 toSample = samplePos - uSpotlight.vsPos;
		vec3 toSampleDir = normalize(toSample);
		float lightDist = length(toSample);
		float rangeSquared = uSpotlight.range * uSpotlight.range;
		float lightDistSquared = lightDist * lightDist;
		float attenuation = smoothstep(uSpotlight.softAngleCos, uSpotlight.sharpAngleCos, dot(toSampleDir, uSpotlight.vsDir));
		float scale = attenuation * max((-1.0 / rangeSquared) * (lightDistSquared - rangeSquared), 0);

		factor += sample * scale;
	}
	factor /= float(NUM_SAMPLES);

	float weight = factor * (isect.t2 - isect.t1) / FULL_VISIBILITY_DIST;
	outFragColor = vec4(vec3(weight * uSpotlight.color), 1.0);
#endif


#ifdef MINIMAL_INTERSECTION_TEST_MARCHING
	const int NUM_SAMPLES = 128;
	const float MAX_DIST = 5.0;
	const float FULL_VISIBILITY_DIST = 1.5;

	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 camDir = normalize(vsPos);

	// Ray vs Cone intersection test
	Intersection isect = rayVsFiniteCone(vec3(0), camDir, uSpotlight.vsPos, uSpotlight.vsDir, uSpotlight.softAngleCos, uSpotlight.range);
	if (!isect.hit) {
		discard;
	}

	float startT = isect.t1;
	float endT = min(min(isect.t2, length(vsPos)), MAX_DIST);
	vec4 start = uSpotlight.lightMatrix * vec4(camDir * startT, 1.0);
	vec4 end = uSpotlight.lightMatrix * vec4(camDir * endT, 1.0);

	float interpStep = 1.0 / float(NUM_SAMPLES - 1);

	float factor = 0.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		float interp = interpStep * float(i);

		vec4 samplePos = mix(start, end, interp);
		factor += textureProj(uShadowMapLowRes, samplePos);
	}
	factor /= float(NUM_SAMPLES);

	float weight = factor * (isect.t2 - isect.t1) / FULL_VISIBILITY_DIST;
	outFragColor = vec4(vec3(weight * uSpotlight.color), 1.0);
#endif


#ifdef OPTIMIZED_FIXED_SAMPLING
	const int NUM_SAMPLES = 128;
	const float MAX_DIST = 5.0;
	const float FULL_VISIBILITY_DIST = 1.5;

	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 camDir = normalize(vsPos);

	// Ray vs Cone intersection test
	Intersection isect = rayVsFiniteCone(vec3(0), camDir, uSpotlight.vsPos, uSpotlight.vsDir, uSpotlight.softAngleCos, uSpotlight.range);
	if (!isect.hit) {
		discard;
	}

	// Compute march range
	float startT = isect.t1;
	float endT = min(min(isect.t2, length(vsPos)), MAX_DIST);
	vec3 start = camDir * startT;
	vec3 end = camDir * endT;

	// Precompute as much as possible
	float rangeSquared = uSpotlight.range * uSpotlight.range;

	vec4 startClipSpace = uSpotlight.lightMatrix * vec4(start, 1.0);
	vec3 toStart = start - uSpotlight.vsPos;
	float startDistScale = max((-1.0 / rangeSquared) * (pow(length(toStart), 2) - rangeSquared), 0);

	vec4 endClipSpace = uSpotlight.lightMatrix * vec4(end, 1.0);
	vec3 toEnd = end - uSpotlight.vsPos;
	float endDistScale = max((-1.0 / rangeSquared) * (pow(length(toEnd), 2) - rangeSquared), 0);
	

	float interpStep = 1.0 / float(NUM_SAMPLES - 1);

	float factor = 0.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		float interp = interpStep * float(i);

		// Shadow map sample
		vec4 samplePos = mix(startClipSpace, endClipSpace, interp);
		float sample = textureProj(uShadowMapLowRes, samplePos);

		// Dist scaling
		float distScale = mix(startDistScale, endDistScale, interp);

		factor += sample * distScale;
	}
	factor /= float(NUM_SAMPLES);

	float weight = factor * (isect.t2 - isect.t1) / FULL_VISIBILITY_DIST;
	outFragColor = vec4(vec3(weight * uSpotlight.color), 1.0);
#endif


#ifdef OPTIMIZED_FIXED_SAMPLING_WITH_ATTENUATION
	const int NUM_SAMPLES = 128;
	const float MAX_DIST = 5.0;
	const float FULL_VISIBILITY_DIST = 2.0;

	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 camDir = normalize(vsPos);

	// Ray vs Cone intersection test
	Intersection isect = rayVsFiniteCone(vec3(0), camDir, uSpotlight.vsPos, uSpotlight.vsDir, uSpotlight.softAngleCos, uSpotlight.range);
	if (!isect.hit) {
		discard;
	}

	// Compute march range
	float startT = isect.t1;
	float endT = min(min(isect.t2, length(vsPos)), MAX_DIST);
	vec3 start = camDir * startT;
	vec3 end = camDir * endT;

	// Precompute as much as possible
	float rangeSquared = uSpotlight.range * uSpotlight.range;

	vec4 startClipSpace = uSpotlight.lightMatrix * vec4(start, 1.0);
	vec3 toStart = start - uSpotlight.vsPos;
	float startDistScale = max((-1.0 / rangeSquared) * (pow(length(toStart), 2) - rangeSquared), 0);

	vec4 endClipSpace = uSpotlight.lightMatrix * vec4(end, 1.0);
	vec3 toEnd = end - uSpotlight.vsPos;
	float endDistScale = max((-1.0 / rangeSquared) * (pow(length(toEnd), 2) - rangeSquared), 0);
	

	float interpStep = 1.0 / float(NUM_SAMPLES - 1);

	float factor = 0.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		float interp = interpStep * float(i);

		// Shadow map sample
		vec4 samplePos = mix(startClipSpace, endClipSpace, interp);
		float sample = textureProj(uShadowMapLowRes, samplePos);

		// Attenuation
		vec3 toSampleDir = normalize(mix(toStart, toEnd, interp));
		float attenuation = smoothstep(uSpotlight.softAngleCos, uSpotlight.sharpAngleCos, dot(toSampleDir, uSpotlight.vsDir));

		// Dist scaling
		float distScale = mix(startDistScale, endDistScale, interp);

		factor += sample * distScale * attenuation;
	}
	factor /= float(NUM_SAMPLES);

	float weight = factor * (isect.t2 - isect.t1) / FULL_VISIBILITY_DIST;
	outFragColor = vec4(vec3(weight * uSpotlight.color), 1.0);
#endif


#ifdef OPTIMIZED_DYNAMIC_SAMPLING
	const int MAX_NUM_SAMPLES = 128;
	const float MAX_DIST = 5.0;
	const float FULL_VISIBILITY_DIST = 2.0;
	const vec2 SHADOW_MAP_DIMENSIONS = vec2(256);

	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 camDir = normalize(vsPos);

	// Ray vs Cone intersection test
	Intersection isect = rayVsFiniteCone(vec3(0), camDir, uSpotlight.vsPos, uSpotlight.vsDir, uSpotlight.softAngleCos, uSpotlight.range);
	if (!isect.hit) {
		discard;
	}

	// Compute march range
	float startT = isect.t1;
	float endT = min(min(isect.t2, length(vsPos)), MAX_DIST);
	vec3 start = camDir * startT;
	vec3 end = camDir * endT;

	// Precompute as much as possible
	float rangeSquared = uSpotlight.range * uSpotlight.range;

	vec4 startClipSpace = uSpotlight.lightMatrix * vec4(start, 1.0);
	vec3 toStart = start - uSpotlight.vsPos;
	float startDistScale = max((-1.0 / rangeSquared) * (pow(length(toStart), 2) - rangeSquared), 0);

	vec4 endClipSpace = uSpotlight.lightMatrix * vec4(end, 1.0);
	vec3 toEnd = end - uSpotlight.vsPos;
	float endDistScale = max((-1.0 / rangeSquared) * (pow(length(toEnd), 2) - rangeSquared), 0);
	
	// Calculate how many samples to take
	vec2 diff = abs((endClipSpace.xy / endClipSpace.w) - (startClipSpace.xy / startClipSpace.w));
	vec2 texelDiff = diff * SHADOW_MAP_DIMENSIONS;
	int numSamples = int(max(texelDiff.x, texelDiff.y));
	numSamples = clamp(numSamples, 2, MAX_NUM_SAMPLES);
	float interpStep = 1.0 / float(numSamples - 1);

	float factor = 0.0;
	for (int i = 0; i < numSamples; ++i) {
		float interp = interpStep * float(i);

		// Shadow map sample
		vec4 samplePos = mix(startClipSpace, endClipSpace, interp);
		float sample = textureProj(uShadowMapLowRes, samplePos);

		// Dist scaling
		float distScale = mix(startDistScale, endDistScale, interp);

		factor += sample * distScale;
	}
	factor /= float(numSamples);

	float weight = factor * (isect.t2 - isect.t1) / FULL_VISIBILITY_DIST;
	outFragColor = vec4(vec3(weight * uSpotlight.color), 1.0);
#endif


#ifdef OPTIMIZED_DYNAMIC_SAMPLING_WITH_ATTENUATION
	const int MAX_NUM_SAMPLES = 128;
	const float MAX_DIST = 5.0;
	const float FULL_VISIBILITY_DIST = 2.0;
	const vec2 SHADOW_MAP_DIMENSIONS = vec2(256);

	vec3 vsPos = texture(uPositionTexture, uvCoord).xyz;
	vec3 camDir = normalize(vsPos);

	// Ray vs Cone intersection test
	Intersection isect = rayVsFiniteCone(vec3(0), camDir, uSpotlight.vsPos, uSpotlight.vsDir, uSpotlight.softAngleCos, uSpotlight.range);
	if (!isect.hit) {
		discard;
	}

	// Compute march range
	float startT = isect.t1;
	float endT = min(min(isect.t2, length(vsPos)), MAX_DIST);
	vec3 start = camDir * startT;
	vec3 end = camDir * endT;

	// Precompute as much as possible
	float rangeSquared = uSpotlight.range * uSpotlight.range;

	vec4 startClipSpace = uSpotlight.lightMatrix * vec4(start, 1.0);
	vec3 toStart = start - uSpotlight.vsPos;
	float startDistScale = max((-1.0 / rangeSquared) * (pow(length(toStart), 2) - rangeSquared), 0);

	vec4 endClipSpace = uSpotlight.lightMatrix * vec4(end, 1.0);
	vec3 toEnd = end - uSpotlight.vsPos;
	float endDistScale = max((-1.0 / rangeSquared) * (pow(length(toEnd), 2) - rangeSquared), 0);
	
	// Calculate how many samples to take
	vec2 diff = abs((endClipSpace.xy / endClipSpace.w) - (startClipSpace.xy / startClipSpace.w));
	vec2 texelDiff = diff * SHADOW_MAP_DIMENSIONS;
	int numSamples = int(max(texelDiff.x, texelDiff.y));
	numSamples = clamp(numSamples, 2, MAX_NUM_SAMPLES);
	float interpStep = 1.0 / float(numSamples - 1);

	float factor = 0.0;
	for (int i = 0; i < numSamples; ++i) {
		float interp = interpStep * float(i);

		// Shadow map sample
		vec4 samplePos = mix(startClipSpace, endClipSpace, interp);
		float sample = textureProj(uShadowMapLowRes, samplePos);

		// Attenuation
		vec3 toSampleDir = normalize(mix(toStart, toEnd, interp));
		float attenuation = smoothstep(uSpotlight.softAngleCos, uSpotlight.sharpAngleCos, dot(toSampleDir, uSpotlight.vsDir));

		// Dist scaling
		float distScale = mix(startDistScale, endDistScale, interp);

		factor += sample * distScale * attenuation;
	}
	factor /= float(numSamples);

	float weight = factor * (isect.t2 - isect.t1) / FULL_VISIBILITY_DIST;
	outFragColor = vec4(vec3(weight * uSpotlight.color), 1.0);
#endif
}