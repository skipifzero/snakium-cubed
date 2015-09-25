#pragma once
#ifndef SFZ_SDL_WINDOW_HPP
#define SFZ_SDL_WINDOW_HPP

#include <sfz/math/Vector.hpp>
#include <SDL.h>
#include <initializer_list>

namespace sdl {

using sfz::vec2;

/**
 * @brief Enum wrapper for SDL_WindowFlags.
 * @see https://wiki.libsdl.org/SDL_WindowFlags
 */
enum class WindowFlags : Uint32 {
	FULLSCREEN = SDL_WINDOW_FULLSCREEN,
	FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP,
	OPENGL = SDL_WINDOW_OPENGL,
	SHOWN = SDL_WINDOW_SHOWN,
	HIDDEN = SDL_WINDOW_HIDDEN,
	BORDERLESS = SDL_WINDOW_BORDERLESS,
	RESIZABLE = SDL_WINDOW_RESIZABLE,
	MINIMIZED = SDL_WINDOW_MINIMIZED,
	MAXIMIZED = SDL_WINDOW_MAXIMIZED,
	INPUT_GRABBED = SDL_WINDOW_INPUT_GRABBED,
	INPUT_FOCUS = SDL_WINDOW_INPUT_FOCUS,
	MOUSE_FOCUS = SDL_WINDOW_MOUSE_FOCUS,
	FOREIGN = SDL_WINDOW_FOREIGN,
	ALLOW_HIGHDPI = SDL_WINDOW_ALLOW_HIGHDPI,
	//MOUSE_CAPTURE = SDL_WINDOW_MOUSE_CAPTURE // TODO: Not implemented until SDL 2.0.4
};

/**
 * @brief Class responsible for creating, holding and destroying an SDL window.
 *
 * @see https://wiki.libsdl.org/SDL_CreateWindow
 * @see https://wiki.libsdl.org/SDL_DestroyWindow
 * 
 * @author Peter Hillerström <peter@hstroem.se>
 */
class Window final {
public:
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	SDL_Window* const mPtr;

	// Constructors and destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Window() noexcept = delete;
	Window(const Window&) noexcept = delete;
	Window& operator= (const Window&) noexcept = delete;

	Window(const char* title, int width, int height,
	       std::initializer_list<WindowFlags> flags) noexcept;

	~Window() noexcept;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	int width() const noexcept;
	int height() const noexcept;
	vec2 dimensions() const noexcept;

	int drawableWidth() const noexcept;
	int drawableHeight() const noexcept;
	vec2 drawableDimensions() const noexcept;

	SDL_Surface* surfacePtr() const noexcept;

	// Setters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void setSize(int width, int height) noexcept;
};

} // namespace sfz
#endif