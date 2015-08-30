#pragma once
#ifndef SFZ_GUI_INPUT_HPP
#define SFZ_GUI_INPUT_HPP

#include "sfz/sdl/ButtonState.hpp"
#include "sfz/math/Vector.hpp"

namespace gui {

using sfz::vec2;

enum class KeyInput {
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	ACTIVATE
};

struct InputData {
	bool pointerMoved;
	vec2 pointerPos; // Pointer pos in the same coordinate system as SystemRoot
	sdl::ButtonState pointerState;
	vec2 scrollWheel;
	KeyInput key;
};

} // namespace gui
#endif