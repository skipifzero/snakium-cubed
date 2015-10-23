#include "sfz/gl/Scaler.hpp"

#include <algorithm>

#include "sfz/gl/OpenGL.hpp"
#include "sfz/gl/PostProcessQuad.hpp"
#include "sfz/gl/Program.hpp"

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const char* SIMPLE_1_SAMPLE_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uFragSize;
	uniform vec2 uTexelSize;

	// Output
	out vec4 outFragColor;

	void main()
	{
		outFragColor = texture(uSrcTex, uvCoord);
	}
)";

static const char* GRID_4_SAMPLES_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uFragSize;
	uniform vec2 uTexelSize;

	// Output
	out vec4 outFragColor;

	void main()
	{
		vec2 quartFragSize = uFragSize / 4.0;
		vec2 bottomLeftUV = uvCoord - quartFragSize;
		vec2 bottomRightUV = uvCoord + vec2(quartFragSize.x, -quartFragSize.y);
		vec2 topLeftUV = uvCoord + vec2(-quartFragSize.x, quartFragSize.y);
		vec2 topRightUV = uvCoord + quartFragSize;

		outFragColor = (texture(uSrcTex, bottomLeftUV) + texture(uSrcTex, bottomRightUV)
		                + texture(uSrcTex, topLeftUV) + texture(uSrcTex, topRightUV))/4.0f;
	}
)";

// Scaler: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Scaler::Scaler(ScalingAlgorithm scalingAlgo) noexcept
{
	this->mScalingAlgorithm = (ScalingAlgorithm)(((int32_t)scalingAlgo)+1);
	this->changeScalingAlgorithm(scalingAlgo);
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
	// Bind shader, framebuffer and viewport
	glUseProgram(mProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, dstFBO);
	glViewport(dstViewport.min.x, dstViewport.min.y, dstViewport.width(), dstViewport.height());

	// Bind src texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTex);
	gl::setUniform(mProgram, "uSrcTex", 0);
	glBindSampler(0, mSamplerObject);

	vec2 fragSize = vec2{1.0f} / dstViewport.dimensions();
	gl::setUniform(mProgram, "uFragSize", fragSize);

	vec2 texelSize = vec2{1.0f} / srcDimensions;
	gl::setUniform(mProgram, "uTexelSize", texelSize);

	mQuad.render();

	// Cleanup
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(0, 0);
}

void Scaler::changeScalingAlgorithm(ScalingAlgorithm newAlgo) noexcept
{
	if (mScalingAlgorithm == newAlgo) return;

	mScalingAlgorithm = newAlgo;
	glDeleteSamplers(1, &mSamplerObject);
	glGenSamplers(1, &mSamplerObject);

	switch (mScalingAlgorithm) {
	case ScalingAlgorithm::NEAREST:
		mProgram = gl::Program::postProcessFromSource(SIMPLE_1_SAMPLE_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case ScalingAlgorithm::LINEAR:
		mProgram = gl::Program::postProcessFromSource(SIMPLE_1_SAMPLE_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case ScalingAlgorithm::GRID_4_NEAREST:
		mProgram = gl::Program::postProcessFromSource(GRID_4_SAMPLES_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case ScalingAlgorithm::GRID_4_LINEAR:
		mProgram = gl::Program::postProcessFromSource(GRID_4_SAMPLES_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	default:
		sfz_assert_release_m(false, "Invalid scaling algorithm.");
	}
}

} // namespace sfz