#pragma once
#ifndef SFZ_GL_FRAMEBUFFER_HPP
#define SFZ_GL_FRAMEBUFFER_HPP

#include <cstdint>

#include "sfz/math/Vector.hpp"

namespace gl {

using sfz::vec2;
using sfz::vec2i;
using sfz::vec4;
using std::int32_t;
using std::uint32_t;

// FB enums
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

enum class FBTextureFormat : uint32_t {
	// Unsigned normalized 8-bit int, maps to range [0, 1]
	R_U8,
	RG_U8,
	RGB_U8,
	RGBA_U8,

	// Signed normalized 8-bit int, maps to range [-1, 1]
	R_S8,
	RG_S8,
	RGB_S8,
	RGBA_S8,
	
	// Unsigned non-normalized 8-bit int, maps to normal unsigned integer range [0, 255]
	R_INT_U8,
	RG_INT_U8,
	RGB_INT_U8,
	RGBA_INT_U8,
	
	// Signed non-normalized 8-bit int, maps to normal signed integer range [-128, 127]
	R_INT_S8,
	RG_INT_S8,
	RGB_INT_S8,
	RGBA_INT_S8,
	
	// 32-bit float, maps to normal 32-bit float range
	R_F32,
	RG_F32,
	RGB_F32,
	RGBA_F32,
	
	// 16-bit float, maps to normal 16-bit float range
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

enum class FBTextureFiltering : uint32_t {
	NEAREST,
	LINEAR
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
	~FramebufferBuilder() noexcept = default;

	FramebufferBuilder(vec2i dimensions) noexcept;

	// Component adding methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	FramebufferBuilder& setDimensions(vec2i dimensions) noexcept;
	FramebufferBuilder& addTexture(uint32_t index, FBTextureFormat format, FBTextureFiltering filtering) noexcept;
	FramebufferBuilder& addDepthBuffer(FBDepthFormat format) noexcept;
	FramebufferBuilder& addDepthTexture(FBDepthFormat format) noexcept;
	FramebufferBuilder& addStencilBuffer() noexcept;
	FramebufferBuilder& addStencilTexture() noexcept;

	// Component removing methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	FramebufferBuilder& removeTexture(uint32_t index) noexcept;
	FramebufferBuilder& removeDepthBuffer() noexcept;
	FramebufferBuilder& removeDepthTexture() noexcept;
	FramebufferBuilder& removeStencilBuffer() noexcept;
	FramebufferBuilder& removeStencilTexture() noexcept;

	// Framebuffer building method
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Framebuffer build() const noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool mCreateTexture[8] = { false, false, false, false, false, false, false, false };
	bool mCreateDepthBuffer = false;
	bool mCreateDepthTexture = false;
	bool mCreateStencilBuffer = false;
	bool mCreateStencilTexture = false;
	FBTextureFormat mTextureFormat[8];
	FBDepthFormat mDepthFormat;
	FBTextureFiltering mTextureFiltering[8];
	vec2i mDim{-1};
};

// Shadow Map Framebuffer builder function
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Framebuffer createShadowMap(vec2i dimensions, FBDepthFormat depthFormat, bool pcf = true,
                            vec4 borderColor = vec4{0.0f, 0.0f, 0.0f, 1.0f}) noexcept;

// Framebuffer class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Framebuffer final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Framebuffer() noexcept = default;
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator= (const Framebuffer&) = delete;

	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer& operator= (Framebuffer&& other) noexcept;
	~Framebuffer() noexcept;

	friend class FramebufferBuilder;
	friend Framebuffer createShadowMap(vec2i, FBDepthFormat, bool, vec4) noexcept;

	// State checking
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline bool isValid() const noexcept { return mFBO != 0; }
	inline bool hasTexture(uint32_t index) const noexcept { sfz_assert_debug(index < 8); return mTextures[index] != 0; } 
	inline bool hasDepthBuffer() const noexcept { return mDepthBuffer; }
	inline bool hasDepthTexture() const noexcept { return mDepthTexture; }
	inline bool hasStencilBuffer() const noexcept { return mStencilBuffer; }
	inline bool hasStencilTexture() const noexcept { return mStencilTexture; }

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	uint32_t fbo() const noexcept;
	uint32_t texture(uint32_t index) const noexcept;
	uint32_t depthBuffer() const noexcept;
	uint32_t depthTexture() const noexcept;
	uint32_t stencilBuffer() const noexcept;
	uint32_t stencilTexture() const noexcept;
	inline vec2i dimensions() const noexcept { return mDim; }
	inline int32_t width() const noexcept { return mDim.x; };
	inline int32_t height() const noexcept { return mDim.y; }
	inline vec2 dimensionsFloat() const noexcept { return vec2{(float)mDim.x, (float)mDim.y}; }
	inline float widthFloat() const noexcept { return (float)mDim.x; }
	inline float heightFloat() const noexcept { return (float)mDim.y; }

	// Attach/detach component methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	/*void attachTexture(uint32_t index, uint32_t texture) noexcept;
	void attachDepthBuffer(uint32_t buffer) noexcept;
	void attachDepthTexture(uint32_t texture) noexcept;
	void attachStencilBuffer(uint32_t buffer) noexcept;
	void attachStencilTexture(uint32_t texture) noexcept;

	void detachTexture(uint32_t index, uint32_t texture) noexcept;
	void detachDepthBuffer(uint32_t buffer) noexcept;
	void detachDepthTexture(uint32_t texture) noexcept;
	void detachStencilBuffer(uint32_t buffer) noexcept;
	void detachStencilTexture(uint32_t texture) noexcept;*/

	// Steal/offer components methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	/*
	uint32_t stealTexture(uint32_t index) noexcept;
	uint32_t stealDepthBuffer() noexcept;
	uint32_t stealDepthTexture() noexcept;
	uint32_t stealStencilBuffer() noexcept;
	uint32_t stealStencilTexture() noexcept;

	uint32_t stealDetachTexture(uint32_t index) noexcept;
	uint32_t stealDetachDepthBuffer() noexcept;
	uint32_t stealDetachDepthTexture() noexcept;
	uint32_t stealDetachStencilBuffer() noexcept;
	uint32_t stealDetachStencilTexture() noexcept;

	uint32_t offerTexture(uint32_t index, uint32_t texture) noexcept;
	uint32_t offerDepthBuffer(uint32_t buffer) noexcept;
	uint32_t offerDepthTexture(uint32_t texture) noexcept;
	uint32_t offerStencilBuffer(uint32_t buffer) noexcept;
	uint32_t offerStencilTexture(uint32_t texture) noexcept;

	uint32_t offerAttachTexture(uint32_t index, uint32_t texture) noexcept;
	uint32_t offerAttachDepthBuffer(uint32_t buffer) noexcept;
	uint32_t offerAttachDepthTexture(uint32_t texture) noexcept;
	uint32_t offerAttachStencilBuffer(uint32_t buffer) noexcept;
	uint32_t offerAttachStencilTexture(uint32_t texture) noexcept;
	*/

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	uint32_t mFBO = 0;
	uint32_t mTextures[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint32_t mDepthBuffer = 0;
	uint32_t mDepthTexture = 0;
	uint32_t mStencilBuffer = 0;
	uint32_t mStencilTexture = 0;
	vec2i mDim{-1};
};

} // namespace gl
#endif