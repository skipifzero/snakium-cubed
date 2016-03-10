#pragma once

#include <SDL.h>
#include <initializer_list>

// TODO: REMOVE
#include <sfz/sdl/Session.hpp>

namespace sdl {

/**
 * @brief Flags used to initialize SDL.
 * @see https://wiki.libsdl.org/SDL_Init
 */
/*enum class InitFlags : Uint32 {
	TIMER = SDL_INIT_TIMER,
	AUDIO = SDL_INIT_AUDIO,
	VIDEO = SDL_INIT_VIDEO,
	JOYSTICK = SDL_INIT_JOYSTICK,
	HAPTIC = SDL_INIT_HAPTIC,
	GAMECONTROLLER = SDL_INIT_GAMECONTROLLER,
	EVENTS = SDL_INIT_EVENTS,
	EVERYTHING = SDL_INIT_EVERYTHING,
	NOPARACHUTE = SDL_INIT_NOPARACHUTE
};*/

/**
 * @brief Initializes SDL upon construction and cleans up upon destruction.
 *
 * This object should be created before using any SDL features and must be kept alive until SDL is
 * no longer used.
 *
 * @see https://wiki.libsdl.org/SDL_Init
 * @see https://wiki.libsdl.org/SDL_Quit
 */
class Session2 final {
public:

	Session2() noexcept = delete;
	Session2(const Session2&) noexcept = delete;
	Session2& operator= (const Session2&) noexcept = delete;

	/** @brief Initializes SDL & SDL_mixer with the specified flags. */
	Session2(std::initializer_list<InitFlags> initFlags) noexcept;
	~Session2() noexcept;
};

} // namespace sdl
