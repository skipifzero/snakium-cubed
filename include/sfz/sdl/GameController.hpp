#pragma once
#ifndef SFZ_SDL_GAME_CONTROLLER_HPP
#define SFZ_SDL_GAME_CONTROLLER_HPP

#include <cstdint> // uint8_t, int32_t

#include <SDL.h>
#include <sfz/Assert.hpp>
#include <sfz/math/Vector.hpp>

namespace sdl {

using std::uint8_t;
using std::int32_t;

// GameController structs & enums
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/** Enum representing all possible states a GameController button can be in. */
enum class Button : uint8_t {
	NOT_PRESSED,
	DOWN,
	HELD,
	UP
};

/** Struct representing the state of a GameController at a given point in time. */
struct GameController final {
	Button mButtonA = Button::NOT_PRESSED;
	Button mButtonB = Button::NOT_PRESSED;
	Button mButtonX = Button::NOT_PRESSED;
	Button mButtonY = Button::NOT_PRESSED;

	Button mButtonLeftShoulder = Button::NOT_PRESSED;
	Button mButtonRightShoulder = Button::NOT_PRESSED;
	Button mButtonLeftStick = Button::NOT_PRESSED;
	Button mButtonRightStick = Button::NOT_PRESSED;

	Button mButtonDPADUp = Button::NOT_PRESSED;
	Button mButtonDPADDown = Button::NOT_PRESSED;
	Button mButtonDPADLeft = Button::NOT_PRESSED;
	Button mButtonDPADRight = Button::NOT_PRESSED;

	Button mButtonStart = Button::NOT_PRESSED;
	Button mButtonBack = Button::NOT_PRESSED;
	Button mButtonGuide = Button::NOT_PRESSED;

	/** Range (norm of vector): (middle) [0.0f, 1.0f] (fully-pressed) */
	sfz::vec2 mLeftStick;

	/** Range (norm of vector): (middle) [0.0f, 1.0f] (fully-pressed) */
	sfz::vec2 mRightStick;

	float mLeftStickDeadzone = 0.15f;
	float mRightStickDeadzone = 0.15f;

	/** Range: (not-pressed) [0.0f, 1.0f] (fully-pressed) */
	float mLeftTrigger;
	/** Range: (not-pressed) [0.0f, 1.0f] (fully-pressed) */
	float mRightTrigger;

	float mLeftTriggerDeadzone = 0.05f;
	float mRightTriggerDeadzone = 0.05f;
};

// Update functions to update GameController struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/** Starts the update process, should be called once before updateEvent() each frame. */
void updateStart(GameController& controller) noexcept;

/** Updates state with an SDL Event, should be called once for each event after updateStart(). */
void updateProcessEvent(GameController& controller, const SDL_Event& event) noexcept;

/** Finishes the update process, should be called once after all events have been processed. */
void updateFinish(GameController& controller) noexcept;

} // namespace sdl

#endif