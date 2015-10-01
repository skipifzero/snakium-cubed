#version 330

precision highp float; // required by GLSL spec Sect 4.5.3

in vec3 vsPos;
in vec3 vsNormal;

uniform vec3 uColor;

out vec4 outFragColor;

void main()
{
	vec3 color = uColor;

	/*// Object
	if (color == vec3(0, 1, 1)) {
		color = vec3(0.0, 1.0, 1.0);
	}
	// Bonus object
	if (color == vec3(1, 0, 0)) {
		color = vec3(1.0, 0.0, 0.0);
	}
	// Snake
	if (color == vec3(0, 1, 0.25)) {
		color = vec3(0.0, 1.0, 0.25);
	}
	// Digesting Snake
	if (color == vec3(0, 1, 0.5)) {
		color = vec3(0.0, 1.0, 0.5);
	}*/

	vec3 toCam = normalize(-vsPos);
	outFragColor = vec4(dot(toCam, vsNormal)*color, 1);
}