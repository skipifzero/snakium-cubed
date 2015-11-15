#include "sfz/gl/GaussianBlur.hpp"

#include <algorithm>

#include <sfz/gl/OpenGL.hpp>

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const char* HORIZONTAL_SOURCE = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uSrcTex;
	uniform vec2 uSrcDim;

	const float offsets[9] = float[9](-7.302940716, -5.35180578, -3.403984807, -1.458429517, 0.0, 1.458429517, 3.403984807, 5.35180578, 7.302940716);
	const float weights[9] = float[9](0.0125949685786212, 0.0513831777608629, 0.1359278107392780, 0.2333084327472980, 0.1335712203478790, 0.2333084327472980, 0.1359278107392780, 0.0513831777608629, 0.0125949685786212);

	void main()
	{
		vec4 result = vec4(0.0);

		for (int i = 0; i < 9; ++i) {
			result += texture(uSrcTex, uvCoord + vec2(offsets[i] / uSrcDim.x, 0.0)) * weights[i];
		}

		outFragColor = result;
	}
)";

const char* VERTICAL_SOURCE = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uSrcTex;
	uniform vec2 uSrcDim;

	const float offsets[9] = float[9](-7.302940716, -5.35180578, -3.403984807, -1.458429517, 0.0, 1.458429517, 3.403984807, 5.35180578, 7.302940716);
	const float weights[9] = float[9](0.0125949685786212, 0.0513831777608629, 0.1359278107392780, 0.2333084327472980, 0.1335712203478790, 0.2333084327472980, 0.1359278107392780, 0.0513831777608629, 0.0125949685786212);

	void main()
	{
		vec4 result = vec4(0.0);

		for (int i = 0; i < 9; ++i) {
			result += texture(uSrcTex, uvCoord + vec2(0.0, offsets[i] / uSrcDim.y)) * weights[i];
		}

		outFragColor = result;
	}
)";

// GaussianBlur: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GaussianBlur::GaussianBlur(vec2i dimensions) noexcept
:
	mHorizontalBlurProgram{Program::postProcessFromSource(HORIZONTAL_SOURCE)},
	mVerticalBlurProgram{Program::postProcessFromSource(VERTICAL_SOURCE)},
	mTempFB{dimensions}
{
	glGenSamplers(1, &mSamplerObject);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

GaussianBlur::GaussianBlur(GaussianBlur&& other) noexcept
{
	mHorizontalBlurProgram = std::move(other.mHorizontalBlurProgram);
	mVerticalBlurProgram = std::move(other.mVerticalBlurProgram);
	mTempFB = std::move(other.mTempFB);
	mPostProcessQuad = std::move(other.mPostProcessQuad);
	std::swap(this->mSamplerObject, other.mSamplerObject);
}

GaussianBlur& GaussianBlur::operator= (GaussianBlur&& other) noexcept
{
	mHorizontalBlurProgram = std::move(other.mHorizontalBlurProgram);
	mVerticalBlurProgram = std::move(other.mVerticalBlurProgram);
	mTempFB = std::move(other.mTempFB);
	mPostProcessQuad = std::move(other.mPostProcessQuad);
	std::swap(this->mSamplerObject, other.mSamplerObject);
	return *this;
}

GaussianBlur::~GaussianBlur() noexcept
{
	glDeleteSamplers(1, &mSamplerObject);
}

// GaussianBlur: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void GaussianBlur::apply(uint32_t dstFBO, uint32_t srcTexture, vec2i srcDimensions) noexcept
{
	sfz_assert_debug(srcDimensions == mTempFB.dimensions());
	vec2 srcDimFloat{(float)srcDimensions.x, (float)srcDimensions.y};

	glUseProgram(mHorizontalBlurProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mTempFB.fbo());
	glViewport(0, 0, mTempFB.width(), mTempFB.height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);
	gl::setUniform(mHorizontalBlurProgram, "uSrcTex", 0);
	glBindSampler(0, mSamplerObject);
	gl::setUniform(mHorizontalBlurProgram, "uSrcDim", srcDimFloat);

	mPostProcessQuad.render();

	glUseProgram(mVerticalBlurProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, dstFBO);
	glViewport(0, 0, srcDimensions.x, srcDimensions.y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTempFB.colorTexture());
	gl::setUniform(mVerticalBlurProgram, "uSrcTex", 0);
	glBindSampler(0, mSamplerObject);
	gl::setUniform(mVerticalBlurProgram, "uSrcDim", srcDimFloat);

	mPostProcessQuad.render();

	// Cleanup
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(0, 0);
}

} // namespace gl