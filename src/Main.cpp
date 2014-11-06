#include <SDL.h>
#include <SDL_image.h>

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	SDL_Window* window = SDL_CreateWindow("snakium³",
	                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                                      400, 400, SDL_WINDOW_SHOWN);

	SDL_Delay(2000);

	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}