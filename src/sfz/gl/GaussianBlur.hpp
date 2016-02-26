#pragma once
#ifndef SFZ_GL_GAUSSIAN_BLUR_HPP
#define SFZ_GL_GAUSSIAN_BLUR_HPP

#include <cstdint>

#include <sfz/gl/Framebuffer.hpp>
#include <sfz/gl/PostProcessQuad.hpp>
#include <sfz/gl/Program.hpp>
#include <sfz/math/Vector.hpp>

namespace gl {

using sfz::vec2i;
using std::int32_t;
using std::uint32_t;

class GaussianBlur final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GaussianBlur() = default;
	GaussianBlur(const GaussianBlur&) = delete;
	GaussianBlur& operator= (const GaussianBlur&) = delete;
	
	GaussianBlur(vec2i dimensions, int32_t radius, float sigma) noexcept;
	
	GaussianBlur(GaussianBlur&& other) noexcept;
	GaussianBlur& operator= (GaussianBlur&& other) noexcept;
	~GaussianBlur() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	/**
	 * @brief Applies the box blur filter to the texture and writes it to the specifed fbo
	 * @param srcDimensions the dimensions of the texture, needs to be same as the internal
	 *                      resolution of this BoxBlur object.
	 * @param radius the amount of pixels to sample in each direction
	 */
	void apply(uint32_t dstFBO, uint32_t srcTexture, vec2i srcDimensions, int32_t radius) noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	Program mHorizontalBlurProgram, mVerticalBlurProgram;
	Framebuffer mTempFB;
	PostProcessQuad mPostProcessQuad;
	uint32_t mSamplerObject = 0;
};

void herps(int32_t radius, float sigma) noexcept;

} // namespace sfz
#endif