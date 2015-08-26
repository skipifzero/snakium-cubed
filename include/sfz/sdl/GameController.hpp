#pragma once
#ifndef SFZ_SDL_GAME_CONTROLLER_HPP
#define SFZ_SDL_GAME_CONTROLLER_HPP

#include <cstdint> // uint8_t, int32_t
#include <unordered_map>
#include <vector>

#include <SDL.h>
#include <sfz/math/Vector.hpp>

#include "sfz/sdl/ButtonState.hpp"

namespace sdl {

using std::int32_t;
using sfz::vec2;
using std::unordered_map;
using std::vector;

/** Class representing the state of a GameController at a given point in time. */
class GameController final {
public:
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline SDL_GameController* gameControllerPtr() const noexcept { return mGameControllerPtr; }
	inline int32_t id() const noexcept { return mID; } // Unique static identifier

	ButtonState a = ButtonState::NOT_PRESSED;
	ButtonState b = ButtonState::NOT_PRESSED;
	ButtonState x = ButtonState::NOT_PRESSED;
	ButtonState y = ButtonState::NOT_PRESSED;

	float stickDeadzone = 0.15f;
	float triggerDeadzone = 0.05f;

	vec2 leftStick; // Approximate range (length of vector): [0.0f, 1.0f]
	vec2 rightStick; // Approximate range (length of vector): [0.0f, 1.0f]
	ButtonState leftStickButton = ButtonState::NOT_PRESSED;
	ButtonState rightStickButton = ButtonState::NOT_PRESSED;

	ButtonState leftShoulder = ButtonState::NOT_PRESSED;
	ButtonState rightShoulder = ButtonState::NOT_PRESSED;
	float leftTrigger; // Range: (not-pressed) [0.0f, 1.0f] (fully-pressed)
	float rightTrigger; // Range: (not-pressed) [0.0f, 1.0f] (fully-pressed)

	ButtonState padUp = ButtonState::NOT_PRESSED;
	ButtonState padDown = ButtonState::NOT_PRESSED;
	ButtonState padLeft = ButtonState::NOT_PRESSED;
	ButtonState padRight = ButtonState::NOT_PRESSED;

	ButtonState start = ButtonState::NOT_PRESSED;
	ButtonState back = ButtonState::NOT_PRESSED;
	ButtonState guide = ButtonState::NOT_PRESSED;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GameController() noexcept;
	GameController(const GameController&) = delete;
	GameController& operator= (const GameController&) = delete;
	GameController(GameController&& other) noexcept;
	GameController& operator= (GameController&& other) noexcept;

	/** 0 <= deviceIndex < SDL_NumJoysticks() */
	GameController(int deviceIndex) noexcept;
	~GameController() noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	SDL_GameController* mGameControllerPtr;
	int32_t mID; // The SDL joystick id of this controller, used to identify.
};

// Update functions to update GameController struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void update(unordered_map<int32_t,GameController>& controllers, const vector<SDL_Event>& events) noexcept;

} // namespace sdl
#endif