#pragma once
#ifndef SFZ_GL_TEXTURE_HPP
#define SFZ_GL_TEXTURE_HPP

#include <cstdint>
#include <string>

#include "sfz/gl/TextureEnums.hpp"

namespace gl {

using std::string;
using std::uint32_t;

class Texture final {
public:
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	uint32_t handle;
	
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Texture() = delete;
	Texture(const Texture&) = delete;
	Texture& operator= (const Texture&) = delete;
	
	Texture(const string& path, TextureFormat format = TextureFormat::RGBA,
	        TextureFiltering filtering = TextureFiltering::ANISOTROPIC_16) noexcept;
	Texture(const char* path, TextureFormat format = TextureFormat::RGBA,
	        TextureFiltering filtering = TextureFiltering::ANISOTROPIC_16) noexcept;
	Texture(Texture&& other) noexcept;
	Texture& operator= (Texture&& other) noexcept;
	~Texture() noexcept;
};

} // namespace gl
#endif