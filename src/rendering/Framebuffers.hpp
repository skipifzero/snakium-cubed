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

// ExternalFB class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class ExternalFB final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ExternalFB() noexcept = default;
	ExternalFB(const ExternalFB&) = delete;
	ExternalFB& operator= (const ExternalFB&) = delete;

	ExternalFB(vec2 dimensions) noexcept;
	ExternalFB(vec2i dimensions) noexcept;
	
	ExternalFB(ExternalFB&& other) noexcept;
	ExternalFB& operator= (ExternalFB&& other) noexcept;
	~ExternalFB() noexcept;
	
	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline uint32_t fbo() const noexcept { return mFBO; }
	inline uint32_t colorTexture() const noexcept { return mColorTexture; }
	inline vec2 dimensions() const noexcept { return vec2{(float)mDim.x, (float)mDim.y}; }
	inline vec2i dimensionsInt() const noexcept { return mDim; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	uint32_t mFBO = 0;
	uint32_t mDepthBuffer = 0;
	uint32_t mColorTexture = 0;
	vec2i mDim{-1};
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

} // namespace s3
#endif