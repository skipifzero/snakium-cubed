#pragma once
#ifndef S3_RENDERING_FRAMEBUFFERS_HPP
#define S3_RENDERING_FRAMEBUFFERS_HPP

#include <cstdint>

#include <sfz/math/Vector.hpp>

namespace s3 {

using sfz::vec4;
using std::int32_t;
using std::uint32_t;

// GBuffer FBO
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct GBuffer final {
	uint32_t mFBO, mDepthBuffer, mDiffuseTexture, mPositionTexture, mNormalTexture, mEmissiveTexture,
	         mMaterialTexture;
	int mWidth, mHeight;

	GBuffer() = delete;
	GBuffer(const GBuffer&) = delete;
	GBuffer& operator= (const GBuffer&) = delete;

	GBuffer(int width, int height) noexcept;
	GBuffer(GBuffer&& other) noexcept;
	GBuffer& operator= (GBuffer&& other) noexcept;
	~GBuffer() noexcept;
};

// Post Process FBO
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct PostProcessFramebuffer final {
	uint32_t mFBO, mTexture;
	int mWidth, mHeight;

	PostProcessFramebuffer() = delete;
	PostProcessFramebuffer(const PostProcessFramebuffer&) = delete;
	PostProcessFramebuffer& operator= (const PostProcessFramebuffer&) = delete;

	PostProcessFramebuffer(int width, int height) noexcept;
	PostProcessFramebuffer(PostProcessFramebuffer&& other) noexcept;
	PostProcessFramebuffer& operator= (PostProcessFramebuffer&& other) noexcept;
	~PostProcessFramebuffer() noexcept;
};

// Shadow Map FBO
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/** Enum used to set the amount of bits per depth value in a ShadowMap. */
enum class ShadowMapRes : int32_t {
	BITS_16,
	BITS_24,
	BITS_32
};

/** An OpenGL ShadowMap FBO. */
struct ShadowMap {
	uint32_t mFBO, mDepthTexture;
	int mResolution;
	bool mHasPCF;

	ShadowMap() = delete;
	ShadowMap(const ShadowMap&) = delete;
	ShadowMap& operator= (const ShadowMap&) = delete;

	ShadowMap(int resolution, ShadowMapRes depthRes, bool pcf, const vec4& borderColor);
	~ShadowMap();
};

} // namespace s3
#endif