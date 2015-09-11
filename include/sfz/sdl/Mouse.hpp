#pragma once
#ifndef SFZ_SDL_MOUSE_HPP
#define SFZ_SDL_MOUSE_HPP

#include <vector>

#include <SDL.h>
#include <sfz/geometry/AABB2D.hpp>
#include <sfz/math/Vector.hpp>

#include "sfz/sdl/ButtonState.hpp"
#include "sfz/sdl/Window.hpp"

namespace sdl {

using std::vector;

using sfz::AABB2D;
using sfz::vec2;

// Mouse structs
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct Mouse final {
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ButtonState leftButton = ButtonState::NOT_PRESSED;
	ButtonState rightButton = ButtonState::NOT_PRESSED;
	ButtonState middleButton = ButtonState::NOT_PRESSED;

	/*
	 * A raw position should be in the range [0, 1] where (0,0) is the bottom left corner.
	 * In a scaled mouse from "scaleMouse()" the position should be in the specified coordinate
	 * system.
	 */
	vec2 position;
	vec2 motion; // Positive-x: right, Positive-y: up
	vec2 wheel;
	
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Mouse() noexcept = default;
	Mouse(const Mouse&) noexcept = default;
	Mouse& operator= (const Mouse&) noexcept = default;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void update(const Window& window, const vector<SDL_Event>& events) noexcept;
	Mouse scaleMouse(vec2 camPos, vec2 camDim) const noexcept;
	Mouse scaleMouse(const AABB2D& camera) const noexcept;
};

} // namespace sdl
#endif