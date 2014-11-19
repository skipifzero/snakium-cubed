#pragma once
#ifndef SFZ_SDL_GLTEXTURE_HPP
#define SFZ_SDL_GLTEXTURE_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#include <string>
#include <cstring> // std::memcpy
#include <iostream>
#include <exception> // std::terminate
#include "sfz/gl/Utils.hpp"

namespace sdl {

class GLTexture final {
public:
	
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	GLuint mHandle;
	
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	// No default constructor.
	GLTexture() = delete;
	
	// No copy constructor.
	GLTexture(const GLTexture&) = delete;
	
	GLTexture(const std::string& path) noexcept;
	
	~GLTexture() noexcept;
	
	// Operators
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// No assignment operator.
	GLTexture& operator= (const GLTexture&) = delete;
};

} // namespace sdl

#endif