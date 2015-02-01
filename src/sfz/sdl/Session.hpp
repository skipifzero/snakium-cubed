#pragma once
#ifndef SFZ_SDL_SESSION_HPP
#define SFZ_SDL_SESSION_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <initializer_list>
#include <iostream>
#include <exception> // std::terminate

#include "sfz/MSVC12HackON.hpp"

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
 * @brief Flags used to initialize SDL_image.
 * @see http://www.libsdl.org/projects/SDL_image/docs/SDL_image_8.html
 */
enum class ImgInitFlags : int {
	JPG = IMG_INIT_JPG,
	PNG = IMG_INIT_PNG,
	TIF = IMG_INIT_TIF
};

/**
 * @brief Initializes SDL and SDL_image upon construction and cleans up upon destruction.
 *
 * This object should be created before using any SDL features and must be kept alive until SDL is
 * no longer used.
 *
 * @see https://wiki.libsdl.org/SDL_Init
 * @see https://wiki.libsdl.org/SDL_Quit
 * @see http://www.libsdl.org/projects/SDL_image/docs/SDL_image_8.html
 * @see http://www.libsdl.org/projects/SDL_image/docs/SDL_image_9.html
 */
class Session final {
public:
	// No default constructor.
	Session() = delete;

	// No copy constructor.
	Session(const Session&) = delete;
	
	/**
	 * @brief Initializes SDL & SDL_image with the specified flags.
	 * @param initFlags the InitFlags to pass upon initialization
	 * @param imgFlags the ImgInitFlags to pass upon initialization
	 */
	Session(std::initializer_list<InitFlags> initFlags,
	        std::initializer_list<ImgInitFlags> imgFlags) noexcept;
	
	~Session() noexcept;

	// No assignment.
	Session& operator= (const Session&) = delete;
};

} // namespace sdl

#include "sfz/MSVC12HackOFF.hpp"

#endif