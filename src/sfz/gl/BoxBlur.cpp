#include "sfz/gl/BoxBlur.hpp"

#include <algorithm>

#include <sfz/gl/OpenGL.hpp>

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const char* HORIZONTAL_SOURCE = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uSrcTex;
	uniform vec2 uSrcDim;
	uniform int uRadius;

	void main()
	{
		// Somewhat smarter version with 2 pixels per sample
		vec2 offs = vec2(1.0 / uSrcDim.x, 0.0);
		vec2 halfOffs = offs * 0.5;
		float weight = 1.0 / float((uRadius * 2) + 1);

		vec4 result = vec4(0.0);

		for (int i = 1; i < uRadius; i += 2) {
			vec2 currOffs = (float(i) * offs) + halfOffs;
			result += texture(uSrcTex, uvCoord + currOffs);
			result += texture(uSrcTex, uvCoord - currOffs);
		}

		result *= 2.0; // Linear sampling (2 pixels per fetch)
		result += texture(uSrcTex, uvCoord); // Pixel 0
		result *= weight;
		outFragColor = result;


		/* // Naive version with 1 sample per pixel
		vec2 offs = vec2(1.0 / uSrcDim.x, 0.0);
		float weight = 1.0 / float((uRadius * 2) + 1);

		vec4 result = vec4(0.0);
		for (int i = -uRadius; i <= uRadius; ++i) {
			result += texture(uSrcTex, uvCoord + (float(i)*offs));
		}
		outFragColor = result * weight;*/
	}
)";

static const char* VERTICAL_SOURCE = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uSrcTex;
	uniform vec2 uSrcDim;
	uniform int uRadius;

	void main()
	{
		// Somewhat smarter version with 2 pixels per sample
		vec2 offs = vec2(0.0, 1.0 / uSrcDim.y);
		vec2 halfOffs = offs * 0.5;
		float weight = 1.0 / float((uRadius * 2) + 1);

		vec4 result = vec4(0.0);

		for (int i = 1; i < uRadius; i += 2) {
			vec2 currOffs = (float(i) * offs) + halfOffs;
			result += texture(uSrcTex, uvCoord + currOffs);
			result += texture(uSrcTex, uvCoord - currOffs);
		}

		result *= 2.0; // Linear sampling (2 pixels per fetch)
		result += texture(uSrcTex, uvCoord); // Pixel 0
		result *= weight;
		outFragColor = result;


		/* // Naive version with 1 sample per pixel
		vec2 offs = vec2(0.0, 1.0 / uSrcDim.y);
		float weight = 1.0 / float((uRadius * 2) + 1);

		vec4 result = vec4(0.0);
		for (int i = -uRadius; i <= uRadius; ++i) {
			result += texture(uSrcTex, uvCoord + (float(i)*offs));
		}
		outFragColor = result * weight;*/
	}
)";

// BoxBlur: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

BoxBlur::BoxBlur(vec2i dimensions) noexcept
:
	mHorizontalBlurProgram{Program::postProcessFromSource(HORIZONTAL_SOURCE)},
	mVerticalBlurProgram{Program::postProcessFromSource(VERTICAL_SOURCE)},
	mTempFB{FramebufferBuilder{dimensions}.addTexture(0, FBTextureFormat::RGB_U8, FBTextureFiltering::LINEAR).build()}
	//mTempFB{PostProcessFB::createColor(dimensions)}
{
	glGenSamplers(1, &mSamplerObject);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

BoxBlur::BoxBlur(BoxBlur&& other) noexcept
{
	mHorizontalBlurProgram = std::move(other.mHorizontalBlurProgram);
	mVerticalBlurProgram = std::move(other.mVerticalBlurProgram);
	mTempFB = std::move(other.mTempFB);
	mPostProcessQuad = std::move(other.mPostProcessQuad);
	std::swap(this->mSamplerObject, other.mSamplerObject);
}

BoxBlur& BoxBlur::operator= (BoxBlur&& other) noexcept
{
	mHorizontalBlurProgram = std::move(other.mHorizontalBlurProgram);
	mVerticalBlurProgram = std::move(other.mVerticalBlurProgram);
	mTempFB = std::move(other.mTempFB);
	mPostProcessQuad = std::move(other.mPostProcessQuad);
	std::swap(this->mSamplerObject, other.mSamplerObject);
	return *this;
}

BoxBlur::~BoxBlur() noexcept
{
	glDeleteSamplers(1, &mSamplerObject);
}

// BoxBlur: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void BoxBlur::apply(uint32_t dstFBO, uint32_t srcTexture, vec2i srcDimensions, int32_t radius) noexcept
{
	sfz_assert_debug(srcDimensions == mTempFB.dimensions());
	sfz_assert_debug(((radius % 2) == 0));
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
	gl::setUniform(mHorizontalBlurProgram, "uRadius", radius);

	mPostProcessQuad.render();

	glUseProgram(mVerticalBlurProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, dstFBO);
	glViewport(0, 0, srcDimensions.x, srcDimensions.y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTempFB.texture(0));
	gl::setUniform(mVerticalBlurProgram, "uSrcTex", 0);
	glBindSampler(0, mSamplerObject);
	gl::setUniform(mVerticalBlurProgram, "uSrcDim", srcDimFloat);
	gl::setUniform(mVerticalBlurProgram, "uRadius", radius);

	mPostProcessQuad.render();

	// Cleanup
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(0, 0);
}

} // namespace gl