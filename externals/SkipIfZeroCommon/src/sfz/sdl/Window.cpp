#include "sfz/sdl/Window.hpp"

#include <iostream>
#include <exception> // std::terminate

namespace sdl {

namespace {

Uint32 processFlags(const std::initializer_list<WindowFlags>& flags) noexcept
{
	Uint32 flag = 0;
	for (WindowFlags tempFlag : flags) {
		flag = flag | static_cast<Uint32>(tempFlag);
	}
	return flag;
}

SDL_Window* createWindow(const char* title, int width, int height, Uint32 flags) noexcept
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


Window::Window(const char* title, int width, int height,
               std::initializer_list<WindowFlags> flags) noexcept
:
	mPtr{createWindow(title, width, height, processFlags(flags))}
{
	// Initialization complete.
}

Window::~Window() noexcept
{
	SDL_DestroyWindow(mPtr);
}

// Getters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int Window::width() const noexcept
{
	int width;
	SDL_GetWindowSize(mPtr, &width, nullptr);
	return width;
}

int Window::height() const noexcept
{
	int height;
	SDL_GetWindowSize(mPtr, nullptr, &height);
	return height;
}

vec2 Window::dimensions() const noexcept
{
	int width, height;
	SDL_GetWindowSize(mPtr, &width, &height);
	return vec2{static_cast<float>(width), static_cast<float>(height)};
}

int Window::drawableWidth() const noexcept
{
	int width;
	SDL_GL_GetDrawableSize(mPtr, &width, nullptr);
	return width;
}

int Window::drawableHeight() const noexcept
{
	int height;
	SDL_GL_GetDrawableSize(mPtr, nullptr, &height);
	return height;
}

vec2 Window::drawableDimensions() const noexcept
{
	int width, height;
	SDL_GL_GetDrawableSize(mPtr, &width, &height);
	return vec2{static_cast<float>(width), static_cast<float>(height)};
}

SDL_Surface* Window::surfacePtr() const noexcept
{
	return SDL_GetWindowSurface(mPtr);
}

// Setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Window::setSize(int width, int height) noexcept
{
	assert(width > 0);
	assert(height > 0);
	SDL_SetWindowSize(mPtr, width, height);
}

} // namespace sdl