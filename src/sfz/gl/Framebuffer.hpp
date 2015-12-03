#pragma once
#ifndef SFZ_GL_FRAMEBUFFER_HPP
#define SFZ_GL_FRAMEBUFFER_HPP

#include <cstdint>

#include "sfz/math/Vector.hpp"

namespace gl {

using sfz::vec2i;
using std::uint32_t;

// FB enums
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

enum class FBTextureFormat : uint32_t {
	// Unsigned 8-bit int, maps to range [0, 1]
	R_U8,
	RG_U8,
	RGB_U8,
	RGBA_U8,
	// Signed 8-bit int, maps to range [-1, 1]
	R_S8,
	RG_S8,
	RGB_S8,
	RGBA_S8,
	// 32-bit float
	R_F32,
	RG_F32,
	RGB_F32,
	RGBA_F32,
	// 16-bit float
	R_F16,
	RG_F16,
	RGB_F16,
	RGBA_F16
};

enum class FBDepthFormat : uint32_t {
	F16,
	F24,
	F32
};


// Framebuffer Builder class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Framebuffer;

class FramebufferBuilder final {
public:

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	FramebufferBuilder() noexcept = default;
	FramebufferBuilder(const FramebufferBuilder&) noexcept = default;
	FramebufferBuilder& operator= (const FramebufferBuilder&) noexcept = default;

	// Component adding methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	FramebufferBuilder& setDimensions(vec2i dim) noexcept;
	FramebufferBuilder& addColorTexture(uint32_t index, FBTextureFormat format) noexcept;
	FramebufferBuilder& addDepthBuffer(FBDepthFormat format) noexcept;
	FramebufferBuilder& addDepthTexture(FBDepthFormat format) noexcept;
	FramebufferBuilder& addStencilBuffer() noexcept;
	FramebufferBuilder& addStencilTexture() noexcept;

	// Component removing methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	FramebufferBuilder& removeColorTexture(uint32_t index, FBTextureFormat format) noexcept;
	FramebufferBuilder& removeDepthBuffer(FBDepthFormat format) noexcept;
	FramebufferBuilder& removeDepthTexture(FBDepthFormat format) noexcept;
	FramebufferBuilder& removeStencilBuffer() noexcept;
	FramebufferBuilder& removeStencilTexture() noexcept;

	// Framebuffer building method
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Framebuffer build() noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool mCreateColorTexture[8] = { false, false, false, false, false, false, false, false };
	bool mCreateDepthBuffer = false;
	bool mCreateDepthTexture = false;
	bool mCreateStencilBuffer = false;
	bool mCreateStencilTexture = false;
	FBTextureFormat mColorTextureFormats[8];
	FBDepthFormat mDepthFormat;
	vec2i mDim{-1};
};

// Framebuffer class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Framebuffer final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Framebuffer() noexcept = default;
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator= (const Framebuffer&) = delete;

	Framebuffer(Framebuffer&& other) noexcept; // TODO: Define
	Framebuffer& operator= (Framebuffer&& other) noexcept; // TODO: Define
	~Framebuffer() noexcept; // TODO: Define

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	//inline bool isAvailable(uint32_t index) const noexcept { return mColorTextures[(uint32_t)colTex] != 0; }

	//inline uint32_t access(FBComponent colTex) const noexcept { sfz_assert_debug(isAvailable(colTex));
	//														    return mColorTextures[(uint32_t)colTex]; }

	inline uint32_t fbo() const noexcept { sfz_assert_debug(mFBO != 0); return mFBO; };
	inline uint32_t colorTexture(uint32_t index) const noexcept { sfz_assert_debug(index < 8); sfz_assert_debug(mColorTextures[index] != 0); return mColorTextures[index]; };
	inline uint32_t depthBuffer() const noexcept { sfz_assert_debug(mDepthBuffer != 0); return mDepthBuffer; }
	inline uint32_t depthTexture() const noexcept { sfz_assert_debug(mDepthTexture != 0); return mDepthTexture; }
	inline uint32_t stencilBuffer() const noexcept { sfz_assert_debug(mStencilBuffer != 0); return mStencilBuffer; }
	inline uint32_t stencilTexture() const noexcept { sfz_assert_debug(mStencilTexture != 0); return mStencilTexture; }

	//void attach(FBComponent field, uint32_t component);
	//void detach(FBComponent field, uint32_t component);

	//uint32_t steal(FBComponent field);
	//uint32_t stealAndDetach(FBComponent field);
	
	//void offer(FBComponent field, uint32_t component);
	//void offerAndAttach(FBComponent field, uint32_t component);

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	uint32_t mFBO = 0;
	uint32_t mColorTextures[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint32_t mDepthBuffer = 0;
	uint32_t mDepthTexture = 0;
	uint32_t mStencilBuffer = 0;
	uint32_t mStencilTexture = 0;
	vec2i mDim{-1};

	// Friends
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	friend class FramebufferBuilder;
};

} // namespace gl
#endif