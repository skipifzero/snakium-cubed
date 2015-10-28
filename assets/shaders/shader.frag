#version 330

precision highp float; // required by GLSL spec Sect 4.5.3

in vec3 vsPos;
in vec3 vsNormal;

uniform vec4 uColor;

// wip light uniforms
uniform vec3 uSpotLightPos;
uniform vec3 uSpotLightDir;
uniform float uSpotLightReach;
uniform float uSpotLightAngle;

out vec4 outFragColor;

void main()
{
	vec4 color = uColor;
	vec3 toCam = normalize(-vsPos);
	outFragColor = vec4(dot(toCam, vsNormal)*color.rgb, color.a);

	/*vec3 toCam = normalize(-vsPos);
	vec3 toLight = normalize(uSpotLightPos - vsPos);

	float diffuseIntensity = clamp(dot(toLight, vsNormal), 0.0, 1.0);

	outFragColor = vec4(diffuseIntensity*color.rgb, color.a);/*
}