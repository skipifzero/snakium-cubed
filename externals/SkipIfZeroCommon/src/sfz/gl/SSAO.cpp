#include "sfz/gl/SSAO.hpp"

#include <random>
#include <new>

#include <sfz/Assert.hpp>
#include <sfz/gl/OpenGL.hpp>
#include <sfz/math/MathHelpers.hpp>

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const char* SSAO_SHADER = R"(
	#version 330

	// SSAO implementation using normal oriented hemisphere, inspired by this tutorial:
	// http://john-chapman-graphics.blogspot.se/2013/01/ssao-tutorial.html

	// Input
	in vec2 uvCoord;
	in vec3 nonNormRayDir;

	// Output
	out vec4 occlusionOut;

	// Uniforms
	uniform sampler2D uLinearDepthTexture;
	uniform sampler2D uNormalTexture;
	uniform mat4 uProjMatrix;
	uniform float uFarPlaneDist;

	uniform vec2 uDimensions;
	uniform float uRadius;
	uniform float uMinDepthBias;
	uniform float uOcclusionPower;

	uniform int uKernelSize;
	uniform vec3 uKernel[128];
	uniform vec3 uNoise[16];

	float sampleLinearDepth(vec3 vsPos)
	{
		vec4 offset = uProjMatrix * vec4(vsPos, 1.0);
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + vec2(0.5);
		return texture(uLinearDepthTexture, offset.xy).r;
	}

	void main()
	{
		float linDepth = texture(uLinearDepthTexture, uvCoord).r;
		vec3 vsPos = uFarPlaneDist * linDepth * nonNormRayDir / abs(nonNormRayDir.z);
		vec3 normal = texture(uNormalTexture, uvCoord).xyz;

		// Sample 4x4 noise
		ivec2 noiseCoord = ivec2(uvCoord * uDimensions) % ivec2(4);
		vec3 noiseVec = uNoise[noiseCoord.y * 4 + noiseCoord.x];

		// Calculates matrix to rotate kernel into normal hemisphere using Gram Schmidt process
		vec3 tangent = normalize(noiseVec - normal * dot(noiseVec, normal));
		vec3 bitangent = cross(normal, tangent);
		mat3 kernelRot = mat3(tangent, bitangent, normal);

		// Calculates the minimum and maximum depth values for a sample to count as an occluder
		float sampleMinDepth = linDepth - (uRadius / uFarPlaneDist);
		float sampleMaxDepth = linDepth - (uMinDepthBias / uFarPlaneDist);

		// Samples points and calculates occlusion 
		float occlusion = 0.0;
		for (int i = 0; i < uKernelSize; i++) {
			vec3 samplePos = vsPos + uRadius * (kernelRot * uKernel[i]);
			float sampleDepth = sampleLinearDepth(samplePos);

			// Add occlusion (+1) if sample's depth value is within precalculated range
			if (sampleMinDepth < sampleDepth && sampleDepth < sampleMaxDepth) {
				occlusion += 1.0;
			}
		}
		occlusion = pow(1.0 - (occlusion / uKernelSize), uOcclusionPower);

		occlusionOut = vec4(vec3(occlusion), 1.0);
	}
)";

static const char* HORIZONTAL_BLUR_4_SHADER = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uTexture;
	uniform float uTexelWidth;

	void main()
	{
		vec2 sampleCoord0 = uvCoord + vec2(uTexelWidth * -1.0, 0.0);
		vec2 sampleCoord1 = uvCoord + vec2(uTexelWidth * 0.5, 0.0);

		vec4 result = vec4(0);
		result += texture(uTexture, sampleCoord0);
		result += texture(uTexture, sampleCoord1);
		result *= 0.5;

		outFragColor = result;
	}
)";

static const char* VERTICAL_BLUR_4_SHADER = R"(
	#version 330

	in vec2 uvCoord;
	out vec4 outFragColor;

	uniform sampler2D uTexture;
	uniform float uTexelHeight;

	void main()
	{
		vec2 sampleCoord0 = uvCoord + vec2(0.0, uTexelHeight * -1.0);
		vec2 sampleCoord1 = uvCoord + vec2(0.0, uTexelHeight * 0.5);

		vec4 result = vec4(0);
		result += texture(uTexture, sampleCoord0);
		result += texture(uTexture, sampleCoord1);
		result *= 0.5;

		outFragColor = result;
	}
)";

static void resizeFramebuffers(Framebuffer& occlusionFBO, Framebuffer& tempFBO, vec2i dimensions) noexcept
{
	occlusionFBO = FramebufferBuilder{dimensions}
	              .addTexture(0, FBTextureFormat::R_F32, FBTextureFiltering::LINEAR)
	              .build();
	tempFBO = FramebufferBuilder{dimensions}
	         .addTexture(0, FBTextureFormat::R_F32, FBTextureFiltering::LINEAR)
	         .build();
}

static void generateKernel(vec3* kernelPtr, size_t kernelSize) noexcept
{
	std::random_device rd;
	std::mt19937_64 gen{rd()};
	std::uniform_real_distribution<float> distr1{-1.0f, 1.0f};
	std::uniform_real_distribution<float> distr2{0.0f, 1.0f};

	for (size_t i = 0; i < kernelSize; i++) {
		// Random vector in z+ hemisphere.
		kernelPtr[i] = normalize(vec3{distr1(gen), distr1(gen), distr2(gen)});
		
		// Scale it so it has length between 0 and 1.
		//kernelPtr[i] *= distr2(gen); // Naive solution
		
		// More points closer to base
		float scale = (float)i / (float)kernelSize;
		scale = sfz::lerp(0.1f, 1.0f, scale*scale);
		kernelPtr[i] *= scale;
	}
}

static void generateNoise(vec3* noisePtr) noexcept
{
	std::random_device rd;
	std::mt19937_64 gen{rd()};
	std::uniform_real_distribution<float> distr{-1.0f, 1.0f};

	for (size_t i = 0; i < 16; ++i) {
		noisePtr[i] = normalize(vec3{distr(gen), distr(gen), 0.0f});
	}
}

// SSAO: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

SSAO::SSAO(vec2i dimensions, size_t numSamples, float radius, float minDepthBias,
           float occlusionPower, bool blurOcclusion) noexcept
:
	mSSAOProgram{Program::postProcessFromSource(SSAO_SHADER)},
	mHorizontalBlurProgram{Program::postProcessFromSource(HORIZONTAL_BLUR_4_SHADER)},
	mVerticalBlurProgram{Program::postProcessFromSource(VERTICAL_BLUR_4_SHADER)},

	mDimensions{dimensions},
	mRadius{radius},
	mMinDepthBias{minDepthBias},
	mOcclusionPower{occlusionPower},
	mBlurOcclusion{blurOcclusion},
	
	mKernelSize{numSamples},
	mKernel{new (std::nothrow) vec3[128]},
	mNoise{new (std::nothrow) vec3[16]}
{
	static_assert(sizeof(vec3) == sizeof(float)*3, "vec3 is padded");
	sfz_assert_debug(numSamples <= 128);
	sfz_assert_debug(0 <= minDepthBias);
	sfz_assert_debug(minDepthBias < radius);
	sfz_assert_debug(0 < occlusionPower);
	
	resizeFramebuffers(mOcclusionFBO, mTempFBO, mDimensions);
	generateKernel(mKernel.get(), mKernelSize);
	generateNoise(mNoise.get());
}

// SSAO: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

uint32_t SSAO::calculate(uint32_t linearDepthTex, uint32_t normalTex, const mat4& projMatrix,
                         float farPlaneDist) noexcept
{
	// Occlusion pass
	glUseProgram(mSSAOProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mOcclusionFBO.fbo());
	glViewport(0, 0, mOcclusionFBO.width(), mOcclusionFBO.height());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Texture buffer uniforms
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, linearDepthTex);
	gl::setUniform(mSSAOProgram, "uLinearDepthTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTex);
	gl::setUniform(mSSAOProgram, "uNormalTexture", 1);

	// Other uniforms
	gl::setUniform(mSSAOProgram, "uProjMatrix", projMatrix);
	gl::setUniform(mSSAOProgram, "uInvProjMatrix", inverse(projMatrix));
	gl::setUniform(mSSAOProgram, "uFarPlaneDist", farPlaneDist);

	gl::setUniform(mSSAOProgram, "uDimensions", vec2{(float)mDimensions.x, (float)mDimensions.y});
	gl::setUniform(mSSAOProgram, "uRadius", mRadius);
	gl::setUniform(mSSAOProgram, "uMinDepthBias", mMinDepthBias);
	gl::setUniform(mSSAOProgram, "uOcclusionPower", mOcclusionPower);

	gl::setUniform(mSSAOProgram, "uKernelSize", (int32_t)mKernelSize);
	gl::setUniform(mSSAOProgram, "uKernel", mKernel.get(), mKernelSize);
	gl::setUniform(mSSAOProgram, "uNoise", mNoise.get(), 16);
	
	mPostProcessQuad.render();

	if (mBlurOcclusion) {
		// Horizontal blur pass
		glUseProgram(mHorizontalBlurProgram.handle());
		glBindFramebuffer(GL_FRAMEBUFFER, mTempFBO.fbo());
		glViewport(0, 0, mTempFBO.width(), mTempFBO.height());
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mOcclusionFBO.texture(0));
		gl::setUniform(mHorizontalBlurProgram, "uTexture", 0);
		gl::setUniform(mHorizontalBlurProgram, "uTexelWidth", 1.0f / mDimensions.x);

		mPostProcessQuad.render();

		// Vertical blur pass
		glUseProgram(mVerticalBlurProgram.handle());
		glBindFramebuffer(GL_FRAMEBUFFER, mOcclusionFBO.fbo());
		glViewport(0, 0, mOcclusionFBO.width(), mOcclusionFBO.height());
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTempFBO.texture(0));
		gl::setUniform(mVerticalBlurProgram, "uTexture", 0);
		gl::setUniform(mVerticalBlurProgram, "uTexelHeight", 1.0f / mDimensions.y);

		mPostProcessQuad.render();
	}

	return mOcclusionFBO.texture(0);
}

// SSAO: Setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void SSAO::dimensions(vec2i dim) noexcept
{
	sfz_assert_debug(dim.x > 0);
	sfz_assert_debug(dim.y > 0);
	mDimensions = dim;
	resizeFramebuffers(mOcclusionFBO, mTempFBO, mDimensions);
}

void SSAO::dimensions(int width, int height) noexcept
{
	dimensions(vec2i{width, height});
}

void SSAO::numSamples(size_t numSamples) noexcept
{
	sfz_assert_debug(numSamples <= 128);
	mKernelSize = numSamples;
	generateKernel(mKernel.get(), mKernelSize);
}

void SSAO::radius(float radius) noexcept
{
	sfz_assert_debug(0.0f < radius);
	mRadius = radius;
}

void SSAO::minDepthBias(float minDepthBias) noexcept
{
	sfz_assert_debug(0.0f <= minDepthBias);
	mMinDepthBias = minDepthBias;
}

void SSAO::occlusionPower(float occlusionPower) noexcept
{
	sfz_assert_debug(0.0f < occlusionPower);
	mOcclusionPower = occlusionPower;
}

void SSAO::blurOcclusion(bool blurOcclusion) noexcept
{
	mBlurOcclusion = blurOcclusion;
}

} // namespace gl
