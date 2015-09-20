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

// Perhaps temporary
#include "rendering/Assets.hpp"
#include "screens/S3ItemRenderers.hpp"
#include <sfz/gui/Button.hpp>
#include <sfz/gui/DefaultItemRenderers.hpp>

#undef main // Remove SDL hack until we can get it to compile properly

sdl::WindowFlags fullscreenFlag(int fullscreenMode)
{
	switch (fullscreenMode) {
	case 0: return sdl::WindowFlags::OPENGL; // Minor hack, works fine as long as we need OpenGL.
	case 1: return sdl::WindowFlags::FULLSCREEN_DESKTOP;
	case 2: return sdl::WindowFlags::FULLSCREEN;
	default: sfz_error("Invalid fullscreen mode.");
	}
}

int main()
{
	using namespace sdl;
	using std::string;

	s3::GlobalConfig& cfg = s3::GlobalConfig::INSTANCE();
	cfg.save();

	Session sdlSession{{InitFlags::EVENTS, InitFlags::VIDEO, InitFlags::GAMECONTROLLER}};

	const int numDisplays = SDL_GetNumVideoDisplays();
	if (numDisplays < 0) std::cerr << "SDL_GetNumVideoDisplays() failed: " << SDL_GetError() << std::endl;
	if (cfg.displayIndex >= numDisplays) {
		std::cerr << "Display index " << cfg.displayIndex << " is invalid, number of displays is "
		          << numDisplays << ". Resetting to 0." << std::endl;
		cfg.displayIndex = 0;
		cfg.save();
	}

	if (cfg.msaa > 0) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, cfg.msaa);
	}

	Window window{"snakium³", cfg.windowResolutionX, cfg.windowResolutionY,
	     {WindowFlags::OPENGL, WindowFlags::RESIZABLE, WindowFlags::ALLOW_HIGHDPI,
	      fullscreenFlag(cfg.fullscreenMode)}};

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

	// Initializes GUI rendering
	{
		auto& settings = gui::DefaultRenderersSettings::INSTANCE();
		auto& assets = s3::Assets::INSTANCE();

		settings.spriteBatchPtr = &assets.spriteBatch;
		settings.fontPtr = &assets.fontRenderer;
		
		settings.renderBounds = true;
		settings.boundsTexture = assets.TILE_FACE.handle;

		settings.fontScale = 1.2f;
		settings.fontVerticalOffsetScale = -0.275f;
		//settings.fontColor = sfz::vec4{0.84f, 1.0f, 0.84f, 1.0f};

		//settings.fontBgColor = sfz::vec4{0.37f, 0.72f, 0.37f, 1.0f};


		//gui::Button::rendererFactory = s3::snakiumButtonRendererFactory();
	}

	sfz::runGameLoop(window, std::shared_ptr<sfz::BaseScreen>{new s3::MainMenuScreen{}});

	return 0;
}