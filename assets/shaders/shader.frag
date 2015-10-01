#version 330

precision highp float; // required by GLSL spec Sect 4.5.3

in vec3 vsPos;
in vec3 vsNormal;

uniform vec4 uColor;

out vec4 outFragColor;

void main()
{
	vec4 color = uColor;
	vec3 toCam = normalize(-vsPos);
	outFragColor = vec4(dot(toCam, vsNormal)*color.rgb, color.a);
}