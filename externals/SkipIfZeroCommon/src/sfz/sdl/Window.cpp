#include "sfz/sdl/Window.hpp"

#include "sfz/Assert.hpp"

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
	ptr{createWindow(title, width, height, processFlags(flags))}
{ }

Window::Window(const char* title) noexcept
:
	ptr{createWindow(title, 800, 800, processFlags({WindowFlags::RESIZABLE, WindowFlags::OPENGL,
	                                                WindowFlags::ALLOW_HIGHDPI}))}
{ }

Window::~Window() noexcept
{
	SDL_DestroyWindow(ptr);
}

// Getters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

int Window::width() const noexcept
{
	int width;
	SDL_GetWindowSize(ptr, &width, nullptr);
	return width;
}

int Window::height() const noexcept
{
	int height;
	SDL_GetWindowSize(ptr, nullptr, &height);
	return height;
}

vec2 Window::dimensions() const noexcept
{
	int width, height;
	SDL_GetWindowSize(ptr, &width, &height);
	return vec2{static_cast<float>(width), static_cast<float>(height)};
}

int Window::drawableWidth() const noexcept
{
	int width;
	SDL_GL_GetDrawableSize(ptr, &width, nullptr);
	return width;
}

int Window::drawableHeight() const noexcept
{
	int height;
	SDL_GL_GetDrawableSize(ptr, nullptr, &height);
	return height;
}

vec2 Window::drawableDimensions() const noexcept
{
	int width, height;
	SDL_GL_GetDrawableSize(ptr, &width, &height);
	return vec2{static_cast<float>(width), static_cast<float>(height)};
}

SDL_Surface* Window::surfacePtr() const noexcept
{
	return SDL_GetWindowSurface(ptr);
}

// Setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Window::setSize(int width, int height) noexcept
{
	sfz_assert_debug(width > 0);
	sfz_assert_debug(height > 0);
	SDL_SetWindowSize(ptr, width, height);
}

void Window::setVSync(VSync mode) noexcept
{
	int vsyncInterval;
	switch (mode) {
	case VSync::OFF:
		vsyncInterval = 0;
		break;
	case VSync::ON:
		vsyncInterval = 1;
		break;
	case VSync::SWAP_CONTROL_TEAR:
		vsyncInterval = -1;
		break;
	default:
		sfz_assert_release(false);
	}
	if (SDL_GL_SetSwapInterval(vsyncInterval) < 0) {
		std::cerr << "SDL_GL_SetSwapInterval() failed: " << SDL_GetError() << std::endl;
	}
}

void Window::setFullscreen(Fullscreen mode, int displayIndex) noexcept
{
	Uint32 fullscreenFlags;
	switch (mode) {
	case Fullscreen::OFF:
		fullscreenFlags = 0;
		break;
	case Fullscreen::WINDOWED:
		fullscreenFlags = SDL_WINDOW_FULLSCREEN_DESKTOP;
		break;
	case Fullscreen::EXCLUSIVE:
		{
			// Acquiring the display index to use
			const int numDisplays = SDL_GetNumVideoDisplays();
			if (numDisplays < 0) std::cerr << "SDL_GetNumVideoDisplays() failed: " << SDL_GetError() << std::endl;
			if (displayIndex >= numDisplays) {
				std::cerr << "Invalid display index (" << displayIndex << "), using 0 instead." << std::endl;
				displayIndex = 0;
			}
			// -1 Means that the user wants the currently used screen
			if (displayIndex == -1) {
				displayIndex = SDL_GetWindowDisplayIndex(ptr);
				if (displayIndex < 0) {
					std::cerr << "SDL_GetWindowDisplayIndex() failed: " << SDL_GetError() << "\n, using 0 instead." << std::endl;
					displayIndex = 0;
				}
			}

			// Gets and sets the display mode to that of the wanted screen
			SDL_DisplayMode desktopDisplayMode;
			if (SDL_GetDesktopDisplayMode(displayIndex, &desktopDisplayMode) < 0) {
				std::cerr << "SDL_GetDesktopDisplayMode() failed: " << SDL_GetError() << std::endl;
				std::terminate();
			}
			if (SDL_SetWindowDisplayMode(ptr, &desktopDisplayMode) < 0) {
				std::cerr << "SDL_SetWindowDisplayMode() failed: " << SDL_GetError() << std::endl;
				std::terminate();
			}
		}
		fullscreenFlags = SDL_WINDOW_FULLSCREEN;
		break;
	case Fullscreen::EXCLUSIVE_KEEP_CURRENT_DISPLAY_MODE:
		fullscreenFlags = SDL_WINDOW_FULLSCREEN;
		break;
	default:
		sfz_assert_release(false);
	}
	if (SDL_SetWindowFullscreen(ptr, fullscreenFlags) < 0) {
		std::cerr << "SDL_SetWindowFullscreen() failed: " << SDL_GetError() << std::endl;
	}
}

} // namespace sdl