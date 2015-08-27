#include "S3Shaders.hpp"

#include <sfz/GL.hpp>

namespace s3 {

unsigned int compileStandardShaderProgram() noexcept
{
	GLuint vertexShader = gl::compileVertexShader(R"(
		#version 330

		in vec3 position;
		in vec2 texCoordIn;

		out vec2 texCoord;

		uniform mat4 modelViewProj;

		void main()
		{
			gl_Position = modelViewProj * vec4(position, 1);
			texCoord = texCoordIn;
		}
	)");


	GLuint fragmentShader = gl::compileFragmentShader(R"(
		#version 330

		precision highp float; // required by GLSL spec Sect 4.5.3

		in vec2 texCoord;
		uniform sampler2D tex;
		
		out vec4 fragmentColor;

		void main()
		{
			fragmentColor = texture(tex, texCoord.xy);
		}
	)");


	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glBindAttribLocation(shaderProgram, 0, "position");
	glBindAttribLocation(shaderProgram, 1, "texCoordIn");
	glBindFragDataLocation(shaderProgram, 0, "fragmentColor");

	gl::linkProgram(shaderProgram);


	if (gl::checkAllGLErrors()) {
		std::cerr << "^^^ Above errors caused by shader compiling & linking." << std::endl;
	}
	return shaderProgram;
}

} // namespace s3
