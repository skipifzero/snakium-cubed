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

// Intersection test
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct LineIntersection {
	bool hit;
	float t1, t2;
};

// length(lineDir) == 1
LineIntersection lineVsSphere(vec3 linePos, vec3 lineDir, vec3 spherePos, float sphereRadius)
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
		return LineIntersection(false, 0, 0);
	}

	// Calculate intersections
	float sqrtProd = sqrt(sqrtInner);
	float t1 = (-b - sqrtProd) / 2;
	float t2 = (-b + sqrtProd) / 2;

	return LineIntersection(true, t1, t2);	
}

// length(coneDir) == 1
// length(rayDir) == 1
// cosConeAngle == cos(coneAngle) == cos(coneFov / 2)
LineIntersection lineVsInfiniteCone(vec3 linePos, vec3 lineDir, vec3 conePos, vec3 coneDir, float coneCosAngle)
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
		return LineIntersection(false, 0.0, 0.0);
	}

	// Calculate closest and farthest points
	float sqrtProd = sqrt(sqrtInner);
	float a2 = 2 * a;
	float t1 = (-b - sqrtProd) / a2;
	float t2 = (-b + sqrtProd) / a2;

	return LineIntersection(true, t1, t2);
}

struct Intersection {
	bool hit;
	vec3 first, second;
};

// length(coneDir) == 1
// length(rayDir) == 1
// cosConeAngle == cos(coneAngle) == cos(coneFov / 2)
Intersection rayVsFiniteCone(vec3 rayPos, vec3 rayDir, vec3 conePos, vec3 coneDir, float coneCosAngle, float coneRange)
{
	LineIntersection coneIsect = lineVsInfiniteCone(rayPos, rayDir, conePos, coneDir, coneCosAngle);

	if (!coneIsect.hit) {
		return Intersection(false, vec3(0), vec3(0));
	}

	// Calculate variables to check if ray started outside infinite cone
	vec3 coneToRay = rayPos - conePos;
	vec3 coneToRayDir = normalize(coneToRay);
	bool rayStartsOutsideInfiniteCone = dot(coneToRayDir, coneDir) < coneCosAngle;

	// If ray starts outside infinite cone and both hits are behind start position then it does not hit
	if (rayStartsOutsideInfiniteCone && coneIsect.t1 < 0 && coneIsect.t2 < 0) {
		return Intersection(false, vec3(0), vec3(0));
	}

	vec3 cHit1 = rayPos + rayDir * coneIsect.t1;
	vec3 cHit2 = rayPos + rayDir * coneIsect.t2;

	LineIntersection sphereIsect = lineVsSphere(rayPos, rayDir, conePos, coneRange);
	vec3 sHit1 = rayPos + rayDir * sphereIsect.t1;
	vec3 sHit2 = rayPos + rayDir * sphereIsect.t2;

	// Calculate variables checking whether cone intersections are non-mirror cone and and in range
	vec3 coneToCHit1 = cHit1 - conePos;
	vec3 coneToCHit2 = cHit2 - conePos;
	float coneToCHit1Length = length(coneToCHit1);
	float coneToCHit2Length = length(coneToCHit2);

	bool cHit1OnPosSide = dot(coneToCHit1, coneDir) > 0;
	bool cHit2OnPosSide = dot(coneToCHit2, coneDir) > 0;
	bool cHit1InRange = coneToCHit1Length <= coneRange;
	bool cHit2InRange = coneToCHit2Length <= coneRange;

	
	if (rayStartsOutsideInfiniteCone) {

		// Case 1: Enter through side of cone, exit through side of cone
		if (cHit1OnPosSide && cHit1InRange && cHit2OnPosSide && cHit2InRange) {
			return Intersection(true, cHit1, cHit2);
		}

		// Case 2: Enter through side of cone, exit through sphere part
		// First hits the false mirrored sphere
		if (!cHit1OnPosSide && cHit2OnPosSide && cHit2InRange) {
			return Intersection(true, cHit2, sHit2);
		}

		// Case 3: Enter through side of cone, exit through sphere part
		// Both first and second are on correct side
		if (cHit1OnPosSide && cHit1InRange && cHit2OnPosSide && !cHit2InRange) {
			return Intersection(true, cHit1, sHit2);
		}
		
		// Case 4: Did not hit cone
		return Intersection(false, vec3(0), vec3(0));
	}


	return Intersection(false, vec3(0), vec3(0));


	/*// Simplifications
	float cos2 = coneCosAngle * coneCosAngle;
	float dirsDot = dot(coneDir, rayDir);
	vec3 rayToCone = rayPos - conePos;
	float coneDirDotRayToCone = dot(coneDir, rayToCone);

	// Calculations
	// ax² + bx + c = 0
	// x = (-b [+-] sqrt(b¹ - 4ac)) / (2a)
	float a = (dirsDot * dirsDot) - cos2;
	float b = 2.0 * (coneDirDotRayToCone * dirsDot - cos2 * dot(rayToCone, rayDir));
	float c = coneDirDotRayToCone * coneDirDotRayToCone - cos2 * dot(rayToCone, rayToCone);

	// sqrt(b¹ - 4ac)
	// if inner factor is below zero there is no solution, i.e. no hit
	float sqrtInner = b * b - 4 * a * c;
	if (sqrtInner < 0) {
		return Intersection(false, vec3(0), vec3(0));
	}

	// Calculate closest and farthest points
	float sqrtProd = sqrt(sqrtInner);
	float a2 = 2 * a;
	float t1 = (-b - sqrtProd) / a2;
	float t2 = (-b + sqrtProd) / a2;
	vec3 closest = rayPos + t1 * rayDir;
	vec3 farthest = rayPos + t2 * rayDir;

	// Calculate variables to check if ray started inside cone or infinite cone area
	vec3 coneToRay = rayPos - conePos;
	vec3 coneToRayDir = normalize(coneToRay);
	bool rayStartsOutsideCone = dot(coneToRayDir, coneDir) < coneCosAngle;


	// Calculate variables to check if intersections is on positive cone and in range
	vec3 coneToClosest = closest - conePos;
	vec3 coneToFarthest = farthest - conePos;
	float coneToClosestLength = length(coneToClosest);
	float coneToFarthestLength = length(coneToFarthest);

	bool closestOnPosSide = dot(coneToClosest, coneDir) > 0;
	bool farthestOnPosSide = dot(coneToFarthest, coneDir) > 0;
	bool closestInRange = coneToClosestLength <= coneRange;
	bool farthestInRange = coneToFarthestLength <= coneRange;


	// Sphere intersection
	Intersection sphereIsect = lineVsSphereIntersectionTest(rayPos, rayDir, conePos, coneRange);

	// Ray started outside infinite cone area
	if (rayStartsOutsideCone) {

		// Currently not checking if ray is pointing towards sphere =/

		// Case 1: Enter through side of cone, exit through side of cone
		if (closestOnPosSide && closestInRange && farthestOnPosSide && farthestInRange) {
			return Intersection(true, closest, farthest);
		}

		// Case 2: Enter through side of cone, exit through sphere part
		// Closest hit the false mirrored sphere
		if (!closestOnPosSide && farthestOnPosSide && farthestInRange) {
			return Intersection(true, farthest, sphereIsect.second);
		}

		// Case 3: Enter through side of cone, exit through sphere part
		// Both closest and farthest are on correct side
		if (closestOnPosSide && closestInRange && farthestOnPosSide && !farthestInRange) {
			return Intersection(true, closest, sphereIsect.second);
		}

		
		// Case 4: Did not hit cone
		return Intersection(false, vec3(0), vec3(0));
	}

	// Ray started inside infinite cone area
	else {

		// Check if ray started inside actual cone
		if (length(coneToRay) <= coneRange) {

		}
	}


	return Intersection(false, vec3(0), vec3(0));*/


	

	/*vec3 coneToClosestDir = normalize(coneToClosest);
	vec3 coneToFarthestDir = normalize(coneToFarthest);

	float coneToClosestLength = length(coneToClosest);
	float coneToFarthestLength = length(coneToFarthest);

	bool b1 = dot(coneToClosestDir, coneDir) >= coneCosAngle; // maybe eps


	if (dot(coneToClosestDir, ))

	return Intersection(true, closest, farthest);*/
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

	//outFragColor = vec4(vec3(length(isect.second - isect.first) / length(vsPos)), 1.0);
	outFragColor = vec4(vec3(length(isect.second - isect.first) / lightRange), 1.0);

	/*float outerCos = uSpotlight.softAngleCos;
	float innerCos = uSpotlight.sharpAngleCos;
	float lightShafts = lightShaftFactor(uShadowMapLowRes, vsPos, outerCos, innerCos);

	vec3 total = uLightShaftExposure * lightShafts * uSpotlight.color;
	outFragColor = vec4(total, 1.0);*/
}