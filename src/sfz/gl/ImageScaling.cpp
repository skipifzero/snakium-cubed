#include "sfz/gl/ImageScaling.hpp"

#include "sfz/gl/OpenGL.hpp"
#include "sfz/gl/PostProcessQuad.hpp"
#include "sfz/gl/Program.hpp"

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const char* NEAREST_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;

	// Output
	out vec4 outFragColor;

	void main()
	{
		outFragColor = texture(uSrcTex, uvCoord);
	}
)";

// Scaling functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void scaleCopy(uint32_t dstFBO, vec2 dstDimensions, uint32_t srcTex,
               ResamplingAlgorithm resamplingAlgo) noexcept
{
	scaleCopy(dstFBO, AABB2D{dstDimensions/2.0f, dstDimensions}, srcTex, resamplingAlgo);
}

void scaleCopy(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex,
               ResamplingAlgorithm resamplingAlgo) noexcept
{
	switch (resamplingAlgo) {
	case ResamplingAlgorithm::NEAREST:
		scaleCopyNearest(dstFBO, dstViewport, srcTex);
		return;
	}
	sfz_assert_release_m(false, "Invalid resampling algorithm or enum not handled properly.");
}

void scaleCopyNearest(uint32_t dstFBO, vec2 dstDimensions, uint32_t srcTex) noexcept
{
	scaleCopyNearest(dstFBO, AABB2D{dstDimensions/2.0f, dstDimensions}, srcTex);
}

void scaleCopyNearest(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex) noexcept
{
	static PostProcessQuad quad{};
	static Program program = Program::postProcessFromSource(NEAREST_SHADER_SRC);

	glUseProgram(program.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, dstFBO);
	glViewport(dstViewport.min.x, dstViewport.min.y, dstViewport.width(), dstViewport.height());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTex);
	gl::setUniform(program, "uSrcTex", 0);

	quad.render();
}

} // namespace sfz