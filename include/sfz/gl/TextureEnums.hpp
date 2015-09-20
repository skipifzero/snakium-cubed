#pragma once
#ifndef SFZ_GL_TEXTURE_ENUMS_HPP
#define SFZ_GL_TEXTURE_ENUMS_HPP

#include <cstdint>

namespace gl {

using std::uint8_t;

enum class TextureFormat : uint8_t {
	GRAY = 1, // Gray == color.r
	GRAY_ALPHA = 2, // Gray == color.r, Alpha == color.g
	RGB = 3,
	RGBA = 4
};

enum class TextureFiltering : uint8_t {
	NEAREST = 0,
	BILINEAR = 1,
	TRILINEAR = 2,
	ANISOTROPIC_1 = 3,
	ANISOTROPIC_2 = 4,
	ANISOTROPIC_4 = 5,
	ANISOTROPIC_8 = 6,
	ANISOTROPIC_16 = 7
};

} // namespace gl
#endif