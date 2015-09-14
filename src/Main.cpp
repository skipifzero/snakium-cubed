#include <iostream>
#include <memory>
#include <string>

#include <sfz/Assert.hpp>
#include <sfz/GL.hpp>
#include <sfz/Math.hpp>
#include <sfz/Screens.hpp>
#include <sfz/SDL.hpp>

#include "GlobalConfig.hpp"
#include "Screens.hpp"

#undef main // Remove SDL hack until we can get it to compile properly

int main()
{
	using namespace sdl;
	using std::string;

	s3::GlobalConfig& cfg = s3::GlobalConfig::INSTANCE();

	Session sdlSession{{InitFlags::EVENTS, InitFlags::VIDEO, InitFlags::GAMECONTROLLER}};

	if (cfg.msaa > 0) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, cfg.msaa);
	}

	Window window{"snakium³", cfg.windowResolutionX, cfg.windowResolutionY,
	     {WindowFlags::OPENGL, WindowFlags::RESIZABLE, WindowFlags::ALLOW_HIGHDPI,
	      cfg.fullscreen ? WindowFlags::FULLSCREEN_DESKTOP : WindowFlags::OPENGL}};

	gl::Context glContext{window.mPtr, 3, 3, gl::GLContextProfile::CORE};

	// Initializes GLEW, must happen after GL context is created.
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		sfz_error(string{"GLEW init failure: "} + ((const char*)glewGetErrorString(glewError)) + "\n");
	}
	if (gl::checkAllGLErrors()) std::cerr << "^^^ Above errors caused by glewInit()." << std::endl;

	// Enable/disable vsync
	if (!cfg.vsync) SDL_GL_SetSwapInterval(0);

	sfz::runGameLoop(window, std::shared_ptr<sfz::BaseScreen>{new s3::MainMenuScreen{}});

	return 0;
}