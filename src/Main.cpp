#include <iostream>
#include <exception> // std::terminate()
#include <string>
#include <memory>

#include <sfz/GL.hpp>
#include <sfz/Math.hpp>
#include <sfz/Screens.hpp>
#include <sfz/SDL.hpp>

#include "Camera.hpp"
#include "GameLogic.hpp"
#include "GlobalConfig.hpp"
#include "Rendering.hpp"
#include "Screens.hpp"

#undef main // Remove SDL hack until we can get it to compile properly

void checkGLErrorsMessage(const std::string& msg) noexcept
{
	if (gl::checkAllGLErrors()) std::cerr << msg << std::endl;
}

int main()
{
	// Init global config variable
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	s3::GlobalConfig& cfg = s3::GlobalConfig::INSTANCE();
	cfg.load();
	cfg.save(); // Save the sanitized values to avoid user confusion.

	// Init libraries and stuff
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Session sdlSession{{sdl::InitFlags::EVENTS, sdl::InitFlags::VIDEO}};

	if (cfg.msaa > 0) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, cfg.msaa);
	}

	sdl::Window window{"snakium³", cfg.windowResolutionX, cfg.windowResolutionY,
	     {sdl::WindowFlags::OPENGL, sdl::WindowFlags::RESIZABLE, sdl::WindowFlags::ALLOW_HIGHDPI,
	      cfg.fullscreen ? sdl::WindowFlags::FULLSCREEN_DESKTOP : sdl::WindowFlags::OPENGL}};

	gl::Context glContext{window.mPtr, 3, 3, gl::GLContextProfile::CORE};

	// Initializes GLEW, must happen after GL context is created.
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cerr << "GLEW initialization failure:\n" << glewGetErrorString(glewError) << std::endl;
		std::terminate();
	}
	checkGLErrorsMessage("^^^ Above errors caused by glewInit().");

	// Enable/disable vsync
	if (!cfg.vsync) SDL_GL_SetSwapInterval(0);

	// Game loop
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sfz::runGameLoop(window, std::shared_ptr<sfz::BaseScreen>{new s3::MainMenuScreen{}});

	return 0;
}