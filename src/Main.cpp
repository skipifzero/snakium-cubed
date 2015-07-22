#include <iostream>
#include <exception> // std::terminate()
#include <string>
#include <chrono>
#include <memory>

#include "sfz/GL.hpp"
#include "sfz/Math.hpp"
#include "Assets.hpp"
#include "Camera.hpp"
#include "GameLogic.hpp"
#include "GlobalConfig.hpp"
#include "Rendering.hpp"
#include "Screens.hpp"

#undef main // Remove SDL hack until we can get it to compile properly

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

	std::unique_ptr<s3::IScreen> currentScreen{new s3::GameScreen{window, assets, globalConfig}};

	// Game loop
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool running = true;
	float delta = calculateDelta(); // Call calculateDelta() here to initialize counting.
	std::vector<SDL_Event> events;

	while (running) {
		delta = calculateDelta();

		pollEvents(events);

		currentScreen->update(events, delta);
		s3::IScreen* newScreen = currentScreen->changeScreen();
		if (newScreen != nullptr) {
			currentScreen = std::unique_ptr<s3::IScreen>{newScreen};
			continue;
		}
		if (currentScreen->quit()) running = false;
		currentScreen->render(delta);

		SDL_GL_SwapWindow(window.mPtr);

		// Hack that silences OpenGL warnings from SDL_GL_SwapWindow() on MSVC12 for some reason.
		int val; SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &val); 

		checkGLErrorsMessage("^^^ Above errors likely caused by game loop.");
	}

	return 0;
}