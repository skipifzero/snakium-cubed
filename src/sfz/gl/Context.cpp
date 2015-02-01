#include "sfz/gl/Context.hpp"

#include "sfz/MSVC12HackON.hpp"

namespace gl {

Context::Context(SDL_Window* window, int major, int minor, GLContextProfile profile) noexcept
{
	// Set context attributes
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major) < 0) {
		std::cerr << "Failed to set gl context major version: " << SDL_GetError() << std::endl;
		std::terminate();
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor) < 0) {
		std::cerr << "Failed to set gl context minor version: " << SDL_GetError() << std::endl;
		std::terminate();
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, static_cast<Uint32>(profile)) < 0) {
		std::cerr << "Failed to set gl context profile: " << SDL_GetError() << std::endl;
		std::terminate();
	}

	mContext = SDL_GL_CreateContext(window);
	if (mContext == NULL) {
		std::cerr << "Failed to create GL context: " << SDL_GetError() << std::endl;
		std::terminate();
	}

	if (gl::checkAllGLErrors()) {
		std::cerr << "^^^ Above errors caused by GL context creation." << std::endl;
	}
}

Context::~Context() noexcept
{
	SDL_GL_DeleteContext(mContext);
}

} // namespace gl

#include "sfz/MSVC12HackOFF.hpp"