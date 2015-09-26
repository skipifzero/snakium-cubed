#include "S3Shaders.hpp"

#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>

namespace s3 {

gl::Program compileStandardShaderProgram() noexcept
{
	return gl::Program::fromSource(R"(
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
	)", R"(
		#version 330

		precision highp float; // required by GLSL spec Sect 4.5.3

		in vec2 texCoord;
		uniform sampler2D tex;
		
		out vec4 fragmentColor;

		void main()
		{
			fragmentColor = texture(tex, texCoord.xy);
		}
	)", [](uint32_t shaderProgram) {
		glBindAttribLocation(shaderProgram, 0, "position");
		glBindAttribLocation(shaderProgram, 1, "texCoordIn");
		glBindFragDataLocation(shaderProgram, 0, "fragmentColor");
	});
}

} // namespace s3
