#pragma once
#ifndef SFZ_GL_POST_PROCESS_FB_HPP
#define SFZ_GL_POST_PROCESS_FB_HPP

#include <cstdint>

#include <sfz/gl/TextureEnums.hpp>
#include <sfz/math/Vector.hpp>

namespace gl {

using sfz::vec2i;
using sfz::vec2;
using std::int32_t;
using std::uint32_t;

// Post Process Framebuffer
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class PostProcessFB final {
public:
	// Constructor functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static PostProcessFB createColor(vec2i dimensions,
	                                 TextureFormat colorTexFormat = TextureFormat::RGB) noexcept;

	static PostProcessFB createColorStencil(vec2i dimensions,
	                                        TextureFormat colorTexFormat = TextureFormat::RGB) noexcept;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	PostProcessFB() noexcept = default;
	PostProcessFB(const PostProcessFB&) = delete;
	PostProcessFB& operator= (const PostProcessFB&) = delete;

	PostProcessFB(PostProcessFB&& other) noexcept;
	PostProcessFB& operator= (PostProcessFB&& other) noexcept;
	~PostProcessFB() noexcept;

	// Public functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline bool isValid() const noexcept { return (mFBO != 0 && mColorTexture != 0 && mDim != vec2i{0}); }
	inline bool hasColorTexture() const noexcept { return mColorTexture != 0; }
	inline bool hasStencilBuffer() const noexcept { return mStencilBuffer != 0; }

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline uint32_t fbo() const noexcept { sfz_assert_debug(isValid()); return mFBO; }
	inline uint32_t colorTexture() const noexcept { sfz_assert_debug(hasColorTexture()); return mColorTexture; }
	inline uint32_t stencilBuffer() const noexcept { sfz_assert_debug(hasStencilBuffer()); return mStencilBuffer; }
	inline vec2i dimensions() const noexcept { return mDim; }
	inline vec2 dimensionsFloat() const noexcept { return vec2{(float)mDim.x, (float)mDim.y}; }
	inline int32_t width() const noexcept { return mDim.x; }
	inline int32_t height() const noexcept { return mDim.y; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	uint32_t mFBO = 0;
	uint32_t mColorTexture = 0;
	uint32_t mStencilBuffer = 0;
	vec2i mDim{0};
};

} // namespace gl
#endif