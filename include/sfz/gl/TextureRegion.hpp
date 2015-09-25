#pragma once
#ifndef SFZ_GL_TEXTURE_REGION_HPP
#define SFZ_GL_TEXTURE_REGION_HPP

#include <sfz/math/Vector.hpp>

namespace sfz {

// TextureRegion
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/** @brief A simple struct specifying an area of a texture. */
struct TextureRegion final {

	// Members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	vec2 mUVMin, mUVMax;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline TextureRegion() noexcept = default;
	inline TextureRegion(const TextureRegion&) noexcept = default;
	inline TextureRegion& operator= (const TextureRegion&) noexcept = default;

	inline TextureRegion(vec2 min, vec2 max) noexcept : mUVMin(min), mUVMax(max) { };

	// Methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline vec2 dimensions() const noexcept { return mUVMax - mUVMin; }
};

} // namespace sfz
#endif