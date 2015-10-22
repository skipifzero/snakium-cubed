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

static const char* BILINEAR_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uHalfFragSize;

	// Output
	out vec4 outFragColor;

	void main()
	{
		vec2 bottomLeftUV = uvCoord - uHalfFragSize;
		vec2 bottomRightUV = uvCoord + vec2(uHalfFragSize.x, -uHalfFragSize.y);
		vec2 topLeftUV = uvCoord + vec2(-uHalfFragSize.x, uHalfFragSize.y);
		vec2 topRightUV = uvCoord + uHalfFragSize;

		outFragColor = (texture(uSrcTex, bottomLeftUV) + texture(uSrcTex, bottomRightUV)
		                + texture(uSrcTex, topLeftUV) + texture(uSrcTex, topRightUV))/4.0f;
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
	case ResamplingAlgorithm::BILINEAR:
		scaleCopyBilinear(dstFBO, dstViewport, srcTex);
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

void scaleCopyBilinear(uint32_t dstFBO, vec2 dstDimensions, uint32_t srcTex) noexcept
{
	scaleCopyBilinear(dstFBO, AABB2D{dstDimensions/2.0f, dstDimensions}, srcTex);
}

void scaleCopyBilinear(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex) noexcept
{
	static PostProcessQuad quad{};
	static Program program = Program::postProcessFromSource(BILINEAR_SHADER_SRC);

	glUseProgram(program.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, dstFBO);
	glViewport(dstViewport.min.x, dstViewport.min.y, dstViewport.width(), dstViewport.height());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTex);
	gl::setUniform(program, "uSrcTex", 0);

	vec2 fragSize{1.0f / dstViewport.width(), 1.0f / dstViewport.height()};
	vec2 halfFragSize = fragSize / 2.0f;
	gl::setUniform(program, "uHalfFragSize", halfFragSize);

	quad.render();
}

} // namespace sfz