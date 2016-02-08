#pragma once
#ifndef SFZ_SDL_WINDOW_HPP
#define SFZ_SDL_WINDOW_HPP

#include <initializer_list>
#include <vector>

#include <sfz/math/Vector.hpp>
#include <SDL.h>

namespace sdl {

using sfz::vec2;
using sfz::vec2i;
using std::vector;

// Enums
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

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
	MOUSE_CAPTURE = SDL_WINDOW_MOUSE_CAPTURE
};

enum class VSync : uint8_t {
	OFF = 0,
	ON = 1,
	SWAP_CONTROL_TEAR = 2 // See https://www.opengl.org/registry/specs/EXT/wgl_swap_control_tear.txt
};

enum class Fullscreen : uint8_t {
	OFF = 0,
	WINDOWED = 1,
	EXCLUSIVE = 2,
	EXCLUSIVE_KEEP_CURRENT_DISPLAY_MODE = 3
};

// Window class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

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

	SDL_Window* const ptr;

	// Constructors and destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Window() noexcept = delete;
	Window(const Window&) noexcept = delete;
	Window& operator= (const Window&) noexcept = delete;

	Window(const char* title, int width, int height,
	       std::initializer_list<WindowFlags> flags) noexcept;
	
	/**
	 * @brief Common default constructor
	 * Will init with SDL_WINDOW_RESIZABLE, SDL_WINDOW_OPENGL and SDL_WINDOW_ALLOW_HIGHDPI
	 */
	Window(const char* title) noexcept;

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
	void setVSync(VSync mode) noexcept;

	/** 
	 * @brief Sets fullscreen mode
	 * Display index is only used for Fullscreen::EXCLUSIVE. Default (-1) means that the current
	 * display of the window will be used.
	 */
	void setFullscreen(Fullscreen mode, int displayIndex = -1) noexcept;
};

// Functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

vector<vec2i> getAvailableResolutions() noexcept;

} // namespace sfz
#endif