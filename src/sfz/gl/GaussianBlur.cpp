#include "sfz/gl/GaussianBlur.hpp"

#include <algorithm>
#include <cmath>
#include <new>

#include <sfz/gl/OpenGL.hpp>

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static constexpr int32_t NUM_INTEGRATION_SAMPLES = 8;

static float gaussian(float x, float sigma) noexcept
{
	return std::exp(-(x*x) / (2.0f * sigma * sigma));
}

static void calculateGaussians(float* sampleArray, int32_t radius, float sigma) noexcept
{
	sfz_assert_debug(radius >= 1);
	constexpr float sampleSize = 1.0f / float(NUM_INTEGRATION_SAMPLES);
	constexpr float initialOffset = -0.5f + (sampleSize/2.0f);

	for (int32_t i = 0; i <= radius; ++i) {
		float startX = float(i) + initialOffset;
		sampleArray[i] = 0.0f;
		for (int32_t j = 0; j < NUM_INTEGRATION_SAMPLES; ++j) {
			sampleArray[i] += (sampleSize * gaussian(startX + float(j) * sampleSize, sigma));
		}
	}
}

static void normalizeGaussianSamples(float* sampleArray, int32_t numberOfSamples) noexcept
{
	sfz_assert_debug(numberOfSamples >= 1);

	float totalWeight = sampleArray[0];
	for (int32_t i = 1; i <= numberOfSamples; ++i) {
		totalWeight += (2.0f * sampleArray[i]);
	}
	
	for (int32_t i = 0; i <= numberOfSamples; ++i) {
		sampleArray[i] /= totalWeight;
	}
}

static const char* HORIZONTAL_SOURCE_NAIVE = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uSrcTex;
	uniform vec2 uSrcDim;
	uniform float uGaussianSamples[257];
	uniform int uRadius;

	void main()
	{
		vec2 offs = vec2(1.0 / uSrcDim.x, 0.0);
		vec4 result = uGaussianSamples[0] * texture(uSrcTex, uvCoord);
		for (int i = 1; i <= uRadius; ++i) {
			vec2 sampleOffs = float(i) * offs;
			result += uGaussianSamples[i] * texture(uSrcTex, uvCoord - sampleOffs);
			result += uGaussianSamples[i] * texture(uSrcTex, uvCoord + sampleOffs);
		}
		outFragColor = result;
	}
)";

static const char* VERTICAL_SOURCE_NAIVE = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uSrcTex;
	uniform vec2 uSrcDim;
	uniform float uGaussianSamples[257];
	uniform int uRadius;

	void main()
	{
		vec2 offs = vec2(0.0, 1.0 / uSrcDim.y);
		vec4 result = uGaussianSamples[0] * texture(uSrcTex, uvCoord);
		for (int i = 1; i <= uRadius; ++i) {
			vec2 sampleOffs = float(i) * offs;
			result += uGaussianSamples[i] * texture(uSrcTex, uvCoord - sampleOffs);
			result += uGaussianSamples[i] * texture(uSrcTex, uvCoord + sampleOffs);
		}
		outFragColor = result;
	}
)";

static const char* HORIZONTAL_SOURCE_INTERPOLATED_SAMPLING = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uSrcTex;
	uniform vec2 uSrcDim;
	uniform float uGaussianSamples[257];
	uniform int uRadius;

	void main()
	{
		vec2 offs = vec2(1.0 / uSrcDim.x, 0.0);
		vec2 halfOffs = offs * 0.5;

		vec4 result = uGaussianSamples[0] * texture(uSrcTex, uvCoord);
		for (int i = 1; i <= uRadius; i += 2) {
			vec2 sampleOffs = (float(i) * offs) + halfOffs;
			float weight = uGaussianSamples[i] + uGaussianSamples[i+1];
			result += weight * texture(uSrcTex, uvCoord - sampleOffs);
			result += weight * texture(uSrcTex, uvCoord + sampleOffs);
		}

		outFragColor = result;
	}
)";

static const char* VERTICAL_SOURCE_INTERPOLATED_SAMPLING = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uSrcTex;
	uniform vec2 uSrcDim;
	uniform float uGaussianSamples[257];
	uniform int uRadius;

	void main()
	{
		vec2 offs = vec2(0.0, 1.0 / uSrcDim.y);
		vec2 halfOffs = offs * 0.5;

		vec4 result = uGaussianSamples[0] * texture(uSrcTex, uvCoord);
		for (int i = 1; i <= uRadius; i += 2) {
			vec2 sampleOffs = (float(i) * offs) + halfOffs;
			float weight = uGaussianSamples[i] + uGaussianSamples[i+1];
			result += weight * texture(uSrcTex, uvCoord - sampleOffs);
			result += weight * texture(uSrcTex, uvCoord + sampleOffs);
		}

		outFragColor = result;
	}
)";

// GaussianBlur: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GaussianBlur::GaussianBlur(vec2i dimensions, int32_t radius, float sigma, bool interpolatedSamples) noexcept
:
	mHorizontalBlurProgram{Program::postProcessFromSource(HORIZONTAL_SOURCE_NAIVE)},
	mVerticalBlurProgram{Program::postProcessFromSource(VERTICAL_SOURCE_NAIVE)},
	mHorizontalBlurInterpolatedProgram{Program::postProcessFromSource(HORIZONTAL_SOURCE_INTERPOLATED_SAMPLING)},
	mVerticalBlurInterpolatedProgram{Program::postProcessFromSource(VERTICAL_SOURCE_INTERPOLATED_SAMPLING)},
	mTempFB{FramebufferBuilder{dimensions}.addTexture(0, FBTextureFormat::RGB_U8, FBTextureFiltering::LINEAR).build()}
{
	glGenSamplers(1, &mSamplerObject);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);

	setBlurParams(radius, sigma, interpolatedSamples);
}

GaussianBlur::GaussianBlur(GaussianBlur&& other) noexcept
{
	mHorizontalBlurProgram = std::move(other.mHorizontalBlurProgram);
	mVerticalBlurProgram = std::move(other.mVerticalBlurProgram);
	mHorizontalBlurInterpolatedProgram = std::move(other.mHorizontalBlurInterpolatedProgram);
	mVerticalBlurInterpolatedProgram = std::move(other.mVerticalBlurInterpolatedProgram);
	mTempFB = std::move(other.mTempFB);
	mPostProcessQuad = std::move(other.mPostProcessQuad);
	std::swap(this->mSamplerObject, other.mSamplerObject);

	std::swap(this->mRadius, other.mRadius);
	std::swap(this->mSigma, other.mSigma);
	std::swap(this->mSamples, other.mSamples);
	std::swap(this->mInterpolatedSamples, other.mInterpolatedSamples);
}

GaussianBlur& GaussianBlur::operator= (GaussianBlur&& other) noexcept
{
	mHorizontalBlurProgram = std::move(other.mHorizontalBlurProgram);
	mVerticalBlurProgram = std::move(other.mVerticalBlurProgram);
	mHorizontalBlurInterpolatedProgram = std::move(other.mHorizontalBlurInterpolatedProgram);
	mVerticalBlurInterpolatedProgram = std::move(other.mVerticalBlurInterpolatedProgram);
	mTempFB = std::move(other.mTempFB);
	mPostProcessQuad = std::move(other.mPostProcessQuad);
	std::swap(this->mSamplerObject, other.mSamplerObject);

	std::swap(this->mRadius, other.mRadius);
	std::swap(this->mSigma, other.mSigma);
	std::swap(this->mSamples, other.mSamples);
	std::swap(this->mInterpolatedSamples, other.mInterpolatedSamples);
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

	const auto& horizBlurProgram = mInterpolatedSamples ? mHorizontalBlurInterpolatedProgram : mHorizontalBlurProgram;
	const auto& vertBlurProgram = mInterpolatedSamples ? mVerticalBlurInterpolatedProgram : mVerticalBlurProgram;

	glUseProgram(horizBlurProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mTempFB.fbo());
	glViewport(0, 0, mTempFB.width(), mTempFB.height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);
	glBindSampler(0, mSamplerObject);
	gl::setUniform(horizBlurProgram, "uSrcTex", 0);
	gl::setUniform(horizBlurProgram, "uSrcDim", srcDimFloat);
	gl::setUniform(horizBlurProgram, "uGaussianSamples", &mSamples[0], mRadius);
	gl::setUniform(horizBlurProgram, "uRadius", mRadius);

	mPostProcessQuad.render();

	glUseProgram(vertBlurProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, dstFBO);
	glViewport(0, 0, srcDimensions.x, srcDimensions.y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTempFB.texture(0));
	glBindSampler(0, mSamplerObject);
	gl::setUniform(vertBlurProgram, "uSrcTex", 0);
	gl::setUniform(vertBlurProgram, "uSrcDim", srcDimFloat);
	gl::setUniform(vertBlurProgram, "uGaussianSamples", &mSamples[0], mRadius);
	gl::setUniform(vertBlurProgram, "uRadius", mRadius);

	mPostProcessQuad.render();

	// Cleanup
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(0, 0);
}

bool GaussianBlur::setBlurParams(int32_t radius, float sigma, bool interpolatedSamples) noexcept
{
	sfz_assert_debug(radius > 0);
	sfz_assert_debug(radius < 257);
	sfz_assert_debug((radius % 2) == 0);
	sfz_assert_debug(sigma > 0.0f);

	if (mRadius == radius && mSigma == sigma) {
		return false;
	}

	mRadius = radius;
	mSigma = sigma;
	mSamples = unique_ptr<float[]>{new (std::nothrow) float[radius+1]};
	mInterpolatedSamples = interpolatedSamples;
	calculateGaussians(&mSamples[0], radius, sigma);
	normalizeGaussianSamples(&mSamples[0], radius);
	return true;
}

} // namespace gl