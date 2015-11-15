#pragma once
#ifndef S3_RENDERING_FRAMEBUFFERS_HPP
#define S3_RENDERING_FRAMEBUFFERS_HPP

#include <cstdint>

#include <sfz/math/Vector.hpp>

namespace s3 {

using sfz::vec2;
using sfz::vec2i;
using sfz::vec4;
using std::int32_t;
using std::uint32_t;

// GBuffer class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class GBuffer final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GBuffer() noexcept = default;
	GBuffer(const GBuffer&) = delete;
	GBuffer& operator= (const GBuffer&) = delete;

	GBuffer(vec2 dimensions) noexcept;
	GBuffer(vec2i dimensions) noexcept;
	
	GBuffer(GBuffer&& other) noexcept;
	GBuffer& operator= (GBuffer&& other) noexcept;
	~GBuffer() noexcept;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline uint32_t fbo() const noexcept { return mFBO; }
	inline uint32_t positionTexture() const noexcept { return mPositionTexture; }
	inline uint32_t normalTexture() const noexcept { return mNormalTexture; }
	inline uint32_t emissiveTexture() const noexcept { return mEmissiveTexture; }
	inline uint32_t materialIdTexture() const noexcept { return mMaterialIdTexture; }
	inline vec2 dimensions() const noexcept { return vec2{(float)mDim.x, (float)mDim.y}; }
	inline vec2i dimensionsInt() const noexcept { return mDim; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	uint32_t mFBO = 0;
	uint32_t mDepthBuffer = 0;
	uint32_t mPositionTexture = 0;
	uint32_t mNormalTexture = 0;
	uint32_t mEmissiveTexture = 0;
	uint32_t mMaterialIdTexture = 0;
	vec2i mDim{-1};
};

} // namespace s3
#endif