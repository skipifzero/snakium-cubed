#include "sfz/sdl/Session.hpp"

#include <iostream>
#include <exception> // std::terminate

namespace sdl {
		
Session::Session(std::initializer_list<InitFlags> initFlags) noexcept
{
	// Initializes SDL
	Uint32 initFlag = 0;
	for (InitFlags tempFlag : initFlags) {
		initFlag = initFlag | static_cast<Uint32>(tempFlag);
	}
	if (SDL_Init(initFlag) < 0) {
		std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;;
		std::terminate();
	}
}
	
Session::~Session() noexcept
{
	SDL_Quit();
}

} // namespace sdl