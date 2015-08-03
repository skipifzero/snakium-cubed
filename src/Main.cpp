#include <iostream>
#include <exception> // std::terminate()
#include <string>
#include <chrono>
#include <memory>

#include <sfz/GL.hpp>
#include <sfz/Math.hpp>
#include <sfz/Screens.hpp>

#include "Assets.hpp"
#include "Camera.hpp"
#include "GameLogic.hpp"
#include "GlobalConfig.hpp"
#include "Rendering.hpp"
#include "Screens.hpp"

#undef main // Remove SDL hack until we can get it to compile properly

using std::shared_ptr;


float calculateDelta() noexcept
{
	static std::chrono::high_resolution_clock::time_point previousTime, currentTime;

	previousTime = currentTime;
	currentTime = std::chrono::high_resolution_clock::now();

	using FloatSecondDuration = std::chrono::duration<float>;
	return std::chrono::duration_cast<FloatSecondDuration>(currentTime - previousTime).count();
}

void checkGLErrorsMessage(const std::string& msg) noexcept
{
	if (gl::checkAllGLErrors()) std::cerr << msg << std::endl;
}

void pollEvents(std::vector<SDL_Event>& events)
{
	events.clear();
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
		default:
			events.push_back(event);
			break;
		}
	}
}

int main()
{
	// Init global config variable
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	s3::GlobalConfig globalConfig;
	globalConfig.load();
	globalConfig.save(); // Save the sanitized values to avoid user confusion.

	// Init libraries and stuff
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Session sdlSession{{sdl::InitFlags::EVENTS, sdl::InitFlags::VIDEO}};

	if (globalConfig.mMSAA > 0) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, globalConfig.mMSAA);
	}

	sdl::Window window{"snakium³", globalConfig.mWindowResolutionX, globalConfig.mWindowResolutionY,
	     {sdl::WindowFlags::OPENGL, sdl::WindowFlags::RESIZABLE, sdl::WindowFlags::ALLOW_HIGHDPI,
	      globalConfig.mFullscreen ? sdl::WindowFlags::FULLSCREEN_DESKTOP : sdl::WindowFlags::OPENGL}};

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
	if (!globalConfig.mVSync) SDL_GL_SetSwapInterval(0);

	// Init variables
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	s3::Assets assets;

	// Game loop
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sfz::runGameLoop(window, std::shared_ptr<sfz::BaseScreen>{new s3::MainMenuScreen{window, assets}});

	return 0;
}