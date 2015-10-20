#pragma once
#ifndef SFZ_GL_SHADOW_MAP_FB_HPP
#define SFZ_GL_SHADOW_MAP_FB_HPP

#include <cstdint>

#include "sfz/math/Vector.hpp"

namespace sfz {

using std::uint32_t;

// Shadow Map FB
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/** Enum used to set the amount of bits per depth value in a ShadowMap. */
enum class ShadowMapDepthRes : int32_t {
	BITS_16,
	BITS_24,
	BITS_32
};

/** An OpenGL ShadowMap FBO. */
class ShadowMapFB {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ShadowMapFB() noexcept = default;
	ShadowMapFB(const ShadowMapFB&) = delete;
	ShadowMapFB& operator= (const ShadowMapFB&) = delete;

	ShadowMapFB(vec2i resolution, ShadowMapDepthRes depthRes = ShadowMapDepthRes::BITS_32,
	            bool pcf = true, vec4 borderColor = vec4(0.0f, 0.0f, 0.0f, 1.0f)) noexcept;
	ShadowMapFB(vec2 resolution, ShadowMapDepthRes depthRes = ShadowMapDepthRes::BITS_32,
	            bool pcf = true, vec4 borderColor = vec4(0.0f, 0.0f, 0.0f, 1.0f)) noexcept;

	ShadowMapFB(ShadowMapFB&& other) noexcept;
	ShadowMapFB& operator= (ShadowMapFB&& other) noexcept;
	~ShadowMapFB() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline bool isValid() const noexcept { return (mFBO != 0 && mDepthTexture != 0); }

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline uint32_t fbo() const noexcept { return mFBO; }
	inline uint32_t depthTexture() const noexcept { return mDepthTexture; }
	inline bool hasPCF() const noexcept { return mHasPCF; }
	inline vec2 resolution() const noexcept { return vec2{(float)mResolution.x, (float)mResolution.y}; }
	inline vec2i resolutionInt() const noexcept { return mResolution; }

private: 
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	uint32_t mFBO = 0;
	uint32_t mDepthTexture = 0;
	bool mHasPCF = false;
	vec2i mResolution{-1,-1};
};

} // namespace sfz
#endif