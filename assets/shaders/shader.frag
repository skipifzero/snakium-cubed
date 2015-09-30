#version 330

precision highp float; // required by GLSL spec Sect 4.5.3

in vec3 vsPos;
in vec3 vsNormal;

out vec4 fragmentColor;

void main()
{
	//fragmentColor = vec4(0.0, 1.0, 1.0, 1.0);
	//fragmentColor = vec4(vsNormal, 1.0);
	gl_FragColor = vec4(vsNormal, 1.0);
}