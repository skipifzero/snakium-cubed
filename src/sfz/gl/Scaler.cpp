#include "sfz/gl/Scaler.hpp"

#include <algorithm>

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

static void scaleCopyNearest(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex) noexcept
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


static void scaleCopyBilinear(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex) noexcept
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
	vec2 halfFragSize = fragSize / 4.0f;
	gl::setUniform(program, "uHalfFragSize", halfFragSize);

	quad.render();
}

// Scaler: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Scaler::Scaler(ScalingAlgorithm scalingAlgo) noexcept
{
	glGenSamplers(1, &mSamplerObject);
}

Scaler::Scaler(Scaler&& other) noexcept
{
	std::swap(this->mScalingAlgorithm, other.mScalingAlgorithm);
	std::swap(this->mProgram, other.mProgram);
}

Scaler& Scaler::operator= (Scaler&& other) noexcept
{
	std::swap(this->mScalingAlgorithm, other.mScalingAlgorithm);
	std::swap(this->mProgram, other.mProgram);
	return *this;
}

Scaler::~Scaler() noexcept
{
	glDeleteSamplers(1, &mSamplerObject);
}

// Scaler: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Scaler::scale(uint32_t dstFBO, vec2 dstDimensions, uint32_t srcTex, vec2 srcDimensions) noexcept
{
	scale(dstFBO, AABB2D{dstDimensions/2.0f, dstDimensions}, srcTex, srcDimensions);
}

void Scaler::scale(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex, vec2 srcDimensions) noexcept
{
	switch (mScalingAlgorithm) {
	case ScalingAlgorithm::NEAREST:
		scaleCopyNearest(dstFBO, dstViewport, srcTex);
		return;
	case ScalingAlgorithm::BILINEAR:
		scaleCopyBilinear(dstFBO, dstViewport, srcTex);
		return;
	}
	sfz_assert_release_m(false, "Invalid scaling algorithm.");
}

void Scaler::changeScalingAlgorithm(ScalingAlgorithm newAlgo) noexcept
{
	mScalingAlgorithm = newAlgo;

	// TOOD: Fix rest of things
}

} // namespace sfz