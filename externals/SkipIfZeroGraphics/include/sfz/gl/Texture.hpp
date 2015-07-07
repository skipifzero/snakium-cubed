#pragma once
#ifndef SFZ_GL_TEXTURE_HPP
#define SFZ_GL_TEXTURE_HPP

#include <string>
#include "sfz/gl/OpenGL.hpp"
#include "sfz/gl/Utils.hpp"



namespace gl {

using std::string;

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
	
	Texture(const string& path) noexcept;
	Texture& operator= (Texture&& other) noexcept;
	~Texture() noexcept;
};

} // namespace gl


#endif