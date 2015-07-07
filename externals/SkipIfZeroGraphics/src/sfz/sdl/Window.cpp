#include "sfz/sdl/Window.hpp"

namespace sdl {

namespace {

Uint32 processFlags(const std::initializer_list<WindowFlags>& flags)
{
	Uint32 flag = 0;
	for (WindowFlags tempFlag : flags) {
		flag = flag | static_cast<Uint32>(tempFlag);
	}
	return flag;
}

SDL_Window* createWindow(const char* title, int width, int height, Uint32 flags)
{
	SDL_Window* window = NULL;
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                          width, height, flags);
	if (window == NULL) {
		std::cerr << "Could not create SDL window: " << SDL_GetError() << std::endl;
		std::terminate();
	}
	return window;
}

} // namespace

// Constructors and destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


Window::Window(const char* title, int width, int height, std::initializer_list<WindowFlags> flags)
:
	mPtr{createWindow(title, width, height, processFlags(flags))}
{
	// Initialization complete.
}

Window::~Window()
{
	SDL_DestroyWindow(mPtr);
}

// Getters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int Window::width() const
{
	int width;
	SDL_GetWindowSize(mPtr, &width, nullptr);
	return width;
}

int Window::height() const
{
	int height;
	SDL_GetWindowSize(mPtr, nullptr, &height);
	return height;
}

int Window::drawableWidth() const
{
	int width;
	SDL_GL_GetDrawableSize(mPtr, &width, nullptr);
	return width;
}

int Window::drawableHeight() const
{
	int height;
	SDL_GL_GetDrawableSize(mPtr, nullptr, &height);
	return height;
}

SDL_Surface* Window::surfacePtr() const
{
	return SDL_GetWindowSurface(mPtr);
}

// Setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Window::setSize(int width, int height)
{
	if(width <= 0 || height <= 0) {
		return;
	}
	SDL_SetWindowSize(mPtr, width, height);
}

} // namespace sdl