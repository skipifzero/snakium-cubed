#pragma once
#ifndef SFZ_GL_GAUSSIAN_BLUR_HPP
#define SFZ_GL_GAUSSIAN_BLUR_HPP

#include <cstdint>

#include <sfz/gl/PostProcessQuad.hpp>
#include <sfz/gl/PostProcessFB.hpp>
#include <sfz/gl/Program.hpp>
#include <sfz/math/Vector.hpp>

namespace gl {

using sfz::vec2i;
using std::uint32_t;

class GaussianBlur final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GaussianBlur() = default;
	GaussianBlur(const GaussianBlur&) = delete;
	GaussianBlur& operator= (const GaussianBlur&) = delete;
	
	GaussianBlur(vec2i dimensions) noexcept;
	GaussianBlur(GaussianBlur&& other) noexcept;
	GaussianBlur& operator= (GaussianBlur&& other) noexcept;
	~GaussianBlur() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void apply(uint32_t dstFBO, uint32_t srcTexture, vec2i srcDimensions) noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	Program mHorizontalBlurProgram, mVerticalBlurProgram;
	PostProcessFB mTempFB;
	PostProcessQuad mPostProcessQuad;
	uint32_t mSamplerObject = 0;
};

} // namespace sfz
#endif