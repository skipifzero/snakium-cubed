#include "sfz/gl/Context.hpp"

#include <string>

#include "sfz/Assert.hpp"
#include "sfz/gl/GLUtils.hpp"

namespace gl {

using std::string;

Context::Context(SDL_Window* window, int major, int minor, GLContextProfile profile) noexcept
{
	// Set context attributes
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major) < 0) {
		sfz_error(string{"Failed to set gl context major version: "} + SDL_GetError() + "\n");
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor) < 0) {
		sfz_error(string{"Failed to set gl context minor version: "} + SDL_GetError() + "\n");
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, static_cast<Uint32>(profile)) < 0) {
		sfz_error(string{"Failed to set gl context profile: "} + SDL_GetError() + "\n");
	}

	handle = SDL_GL_CreateContext(window);
	if (handle == NULL) {
		sfz_error(string{"Failed to create GL context: "} + SDL_GetError() + "\n");
	}

	if (gl::checkAllGLErrors()) {
		std::cerr << "^^^ Above errors caused by GL context creation." << std::endl;
	}
}

Context::~Context() noexcept
{
	SDL_GL_DeleteContext(handle);
}


} // namespace gl