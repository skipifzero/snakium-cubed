#include "sfz/sdl/Session.hpp"

namespace sdl {
		
Session::Session(initializer_list<InitFlags> initFlags, initializer_list<ImgInitFlags> imgFlags)
{
	// Initializes SDL
	Uint32 initFlag = 0;
	for (InitFlags tempFlag : initFlags) {
		initFlag = initFlag | static_cast<Uint32>(tempFlag);
	}
	if (SDL_Init(initFlag) < 0) {
		std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;;
		std::terminate();
	}

	// Initializes SDL_image
	int imgFlag = 0;
	for (ImgInitFlags tempFlag : imgFlags) {
		imgFlag = imgFlag | static_cast<int>(tempFlag);
	}
	int imgInitted = IMG_Init(imgFlag);
	if ((imgInitted & imgFlag) != imgFlag) {
		std::cout << "IMG_Init failed: " << IMG_GetError() << std::endl;
		std::terminate();
	}
}
	
Session::~Session()
{
	IMG_Quit();
	SDL_Quit();
}

} // namespace sdl