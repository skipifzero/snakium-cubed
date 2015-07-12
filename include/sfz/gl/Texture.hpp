#pragma once
#ifndef SFZ_GL_TEXTURE_HPP
#define SFZ_GL_TEXTURE_HPP

#include <string>
#include <cstdint>
#include "sfz/gl/OpenGL.hpp"
#include "sfz/gl/Utils.hpp"

namespace gl {

using std::string;
using std::uint8_t;

enum class TextureFormat : uint8_t {
	GRAY = 1, // Gray == color.r
	GRAY_ALPHA = 2, // Gray == color.r, Alpha == color.g
	RGB = 3,
	RGBA = 4
};

class Texture final {
public:
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	GLuint mHandle;
	
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Texture() = delete;
	Texture(const Texture&) = delete;
	Texture& operator= (const Texture&) = delete;
	
	Texture(const string& path, TextureFormat format = TextureFormat::RGBA) noexcept;
	Texture& operator= (Texture&& other) noexcept;
	~Texture() noexcept;
};

} // namespace gl
#endif