#pragma once
#ifndef SFZ_SDL_BUTTON_STATE_HPP
#define SFZ_SDL_BUTTON_STATE_HPP

#include <cstdint>

namespace sdl {

using std::uint8_t;

enum class ButtonState : uint8_t {
	NOT_PRESSED,
	DOWN,
	HELD,
	UP
};

} // namespace sdl
#endif