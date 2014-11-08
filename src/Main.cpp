#include <sfz/SDL.hpp>
#include <GL/glew.h>

#include <iostream>
#include <exception> // std::terminate()

int main(int argc, char** argv)
{
	sdl::Session sdlSession{{sdl::InitFlags::EVERYTHING}, {sdl::ImgInitFlags::PNG}};
	
	// OpenGL 3.2 Core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow("snakium³",
	                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                                      400, 400, SDL_WINDOW_SHOWN);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	// Initializes GLEW, must happen after GL context is created.
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cout << "GLEW initialization failure:\n" << glewGetErrorString(glewError) << std::endl;
		std::terminate();
	}

	SDL_Delay(2000);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	return 0;
}