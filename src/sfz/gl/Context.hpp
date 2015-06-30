#pragma once
#ifndef SFZ_GL_CONTEXT_HPP
#define SFZ_GL_CONTEXT_HPP

#include <SDL.h>
#include <iostream>
#include <exception> // std::terminate
#include "sfz/gl/Utils.hpp"

namespace gl {

/**
 * @brief Enum describing the different types of OpenGL profiles that can be requested.
 */
enum class GLContextProfile : Uint32 { // TODO: Check if actually Uint32
	CORE = SDL_GL_CONTEXT_PROFILE_CORE,
	COMPATIBILITY = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
	ES = SDL_GL_CONTEXT_PROFILE_ES
};


/**
 * @brief Wrapper class responsible for creating and destroying a OpenGL context.
 */
class Context final {
public:

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	SDL_GLContext mContext;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// No default constructor
	Context() = delete;

	// No copy constructor
	Context(const Context&) = delete;

	Context(SDL_Window* window, int major, int minor, GLContextProfile profile) noexcept;

	~Context() noexcept;

	// Member operators
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// No assignment
	Context& operator= (const Context&) = delete;
};

} // namespace gl
#endif