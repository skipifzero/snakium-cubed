#pragma once
#ifndef SFZ_SDL_SESSION_HPP
#define SFZ_SDL_SESSION_HPP

#include <SDL.h>
#include <initializer_list>

namespace sdl {

/**
 * @brief Flags used to initialize SDL.
 * @see https://wiki.libsdl.org/SDL_Init
 */
enum class InitFlags : Uint32 {
	TIMER = SDL_INIT_TIMER,
	AUDIO = SDL_INIT_AUDIO,
	VIDEO = SDL_INIT_VIDEO,
	JOYSTICK = SDL_INIT_JOYSTICK,
	HAPTIC = SDL_INIT_HAPTIC,
	GAMECONTROLLER = SDL_INIT_GAMECONTROLLER,
	EVENTS = SDL_INIT_EVENTS,
	EVERYTHING = SDL_INIT_EVERYTHING,
	NOPARACHUTE = SDL_INIT_NOPARACHUTE
};

/**
 * @brief Initializes SDL upon construction and cleans up upon destruction.
 *
 * This object should be created before using any SDL features and must be kept alive until SDL is
 * no longer used.
 *
 * @see https://wiki.libsdl.org/SDL_Init
 * @see https://wiki.libsdl.org/SDL_Quit
 */
class Session final {
public:

	Session() noexcept = delete;
	Session(const Session&) noexcept = delete;
	Session& operator= (const Session&) noexcept = delete;

	/** @brief Initializes SDL & SDL_image with the specified flags. */
	Session(std::initializer_list<InitFlags> initFlags) noexcept;
	~Session() noexcept;
};

} // namespace sdl
#endif