#pragma once
#ifndef SFZ_GL_TEXTURE_HPP
#define SFZ_GL_TEXTURE_HPP

#include <cstdint>

#include "sfz/gl/TextureEnums.hpp"

namespace gl {

using std::uint32_t;

class Texture final {
public:
	// Constructor functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static Texture fromFile(const char* path, TextureFormat format = TextureFormat::RGBA,
	                        TextureFiltering filtering = TextureFiltering::ANISOTROPIC_16) noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	inline uint32_t handle() const noexcept { return mHandle; }
	inline bool isValid() const noexcept { return (mHandle == 0); }

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Texture() noexcept = default;
	Texture(const Texture&) = delete;
	Texture& operator= (const Texture&) = delete;
	
	Texture(Texture&& other) noexcept;
	Texture& operator= (Texture&& other) noexcept;
	~Texture() noexcept;

private:
	uint32_t mHandle = 0;
};

} // namespace gl
#endif