#include "sfz/sdl/Session2.hpp"

#include <iostream>
#include <exception> // std::terminate

#include <SDL_mixer.h>

namespace sdl {

Session2::Session2(std::initializer_list<InitFlags> initFlags) noexcept
{
	// Initialize SDL
	Uint32 initFlag = 0;
	for (InitFlags tempFlag : initFlags) {
		initFlag = initFlag | static_cast<Uint32>(tempFlag);
	}
	if (SDL_Init(initFlag) < 0) {
		std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;;
		std::terminate();
	}

	// Initialize SDL_mixer
	int mixInitted = Mix_Init(MIX_INIT_MP3);
	if (mixInitted & MIX_INIT_MP3 != MIX_INIT_MP3) {
		std::cerr << "Failed to initialize SDL_mixer with MP3 support: " << Mix_GetError() << std::endl;
		std::terminate();
	}
	// open 44.1KHz, signed 16bit, system byte order, stereo audio, using 1024 byte chunks
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		std::cerr << "Mix_OpenAudio: " << Mix_GetError() << std::endl;
		std::terminate();
	}
}
	
Session2::~Session2() noexcept
{
	// Cleanup SDL_mixer
	Mix_CloseAudio();
	Mix_Quit();
	while (Mix_Init(0)) Mix_Quit();

	// Cleanup SDL
	SDL_Quit();
}

} // namespace sdl