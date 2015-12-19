#include "sfz/gl/Context.hpp"

#include <string>

#include "sfz/Assert.hpp"

namespace gl {

using std::string;

Context::Context(SDL_Window* window, int major, int minor, GLContextProfile profile, bool debug) noexcept
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

	// Set debug context if requested
	if (debug) {
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG) < 0) {
			std::cerr << "Failed to request debug context: " << SDL_GetError() << std::endl;
		}
	}

	handle = SDL_GL_CreateContext(window);
	if (handle == NULL) {
		sfz_error(string{"Failed to create GL context: "} + SDL_GetError() + "\n");
	}
}

Context::~Context() noexcept
{
	SDL_GL_DeleteContext(handle);
}


} // namespace gl