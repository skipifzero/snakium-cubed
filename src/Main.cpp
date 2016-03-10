#include <iostream>
#include <memory>
#include <string>

#include <sfz/Assert.hpp>
#include <sfz/gl/OpenGL.hpp>
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

#include <SDL_mixer.h>
#include <sfz/sdl/Session2.hpp>

int main(int argc, char* argv[])
{
	using namespace sdl;
	
	// Small hack to enable hi-dpi awareness on Windows.
#ifdef _WIN32
	SetProcessDPIAware();
#endif

	s3::GlobalConfig& cfg = s3::GlobalConfig::INSTANCE();
	cfg.save();

	Session2 sdlSession{{InitFlags::EVENTS, InitFlags::VIDEO, InitFlags::AUDIO, InitFlags::GAMECONTROLLER}};

	// Make sure selected display index is valid
	const int numDisplays = SDL_GetNumVideoDisplays();
	if (numDisplays < 0) std::cerr << "SDL_GetNumVideoDisplays() failed: " << SDL_GetError() << std::endl;
	if (cfg.displayIndex >= numDisplays) {
		std::cerr << "Display index " << cfg.displayIndex << " is invalid, number of displays is "
		          << numDisplays << ". Resetting to 0." << std::endl;
		cfg.displayIndex = 0;
		cfg.save();
	}

	Window window("snakium³");

	// Creates OpenGL context, debug if SFZ_NO_DEBUG is not defined
	SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
	const int MAJOR_VERSION = 4;
	const int MINOR_VERSION = 1;
#if !defined(SFZ_NO_DEBUG)
#ifdef _WIN32
	gl::Context glContext{window.ptr, MAJOR_VERSION, MINOR_VERSION, gl::GLContextProfile::COMPATIBILITY, true};
#else
	gl::Context glContext{window.ptr, MAJOR_VERSION, MINOR_VERSION, gl::GLContextProfile::CORE, true};
#endif
#else
#ifdef _WIN32
	gl::Context glContext{window.ptr, MAJOR_VERSION, MINOR_VERSION, gl::GLContextProfile::COMPATIBILITY, false};
#else
	gl::Context glContext{window.ptr, MAJOR_VERSION, MINOR_VERSION, gl::GLContextProfile::CORE, false};
#endif
#endif
	
	gl::printSystemGLInfo();

	// Initializes GLEW, must happen after GL context is created.
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		sfz_error(std::string{"GLEW init failure: "} + ((const char*)glewGetErrorString(glewError)) + "\n");
	}

	// Fullscreen & VSync
	window.setVSync(static_cast<VSync>(cfg.gc.vsync));
	window.setFullscreen(static_cast<Fullscreen>(cfg.fullscreenMode), cfg.displayIndex);

	// Enable OpenGL debug message if in debug mode
#if !defined(SFZ_NO_DEBUG)
	gl::setupDebugMessages(gl::Severity::MEDIUM, gl::Severity::MEDIUM);
#endif

	// Initializes GUI rendering
	{
		auto& settings = gui::DefaultRenderersSettings::INSTANCE();
		auto& assets = s3::Assets::INSTANCE();

		settings.spriteBatchPtr = &assets.spriteBatch;
		settings.fontPtr = &assets.fontRenderer;
		
		settings.renderBounds = false;
		settings.boundsTexture = assets.ATLAS_128.texture();
		settings.boundsRegion = assets.TILE_FACE_REG;

		settings.fontScale = 1.15f;
		settings.fontVerticalOffsetScale = -0.265f;
		
		settings.fontColor = sfz::vec4{0.84f, 1.0f, 0.84f, 1.0f};
		settings.fontSelectedColor = sfz::vec4{0.3f, 0.75f, 0.3f, 1.0f};
		//settings.fontDisabledColor = sfz::vec4{0.4f, 0.4f, 0.4f, 1.0f};

		//settings.fontRenderDualBg = true;
		//settings.fontBgColor = sfz::vec4{0.37f, 0.72f, 0.37f, 1.0f};
		//settings.fontBgSelectedColor = sfz::vec4{0.0f, 0.0f, 0.0f, 1.0f};
		//settings.fontBgDisabledColor = sfz::vec4{0.0f, 0.0f, 0.0f, 1.0f};

		gui::Button::rendererFactory = s3::snakiumButtonRendererFactory();
	}

	sfz::runGameLoop(window, std::shared_ptr<sfz::BaseScreen>{new s3::MainMenuScreen{}});

	// Super ugly hack, need to rethink assets.
	Mix_FreeMusic(s3::Assets::INSTANCE().GAME_MUSIC.ptr);
}