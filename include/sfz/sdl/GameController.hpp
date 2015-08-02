#pragma once
#ifndef SFZ_SDL_GAME_CONTROLLER_HPP
#define SFZ_SDL_GAME_CONTROLLER_HPP

#include <cstdint> // uint8_t, int32_t
#include <unordered_map>
#include <vector>

#include <SDL.h>
#include <sfz/math/Vector.hpp>

namespace sdl {

using std::uint8_t;
using std::int32_t;
using sfz::vec2;
using std::unordered_map;
using std::vector;

// GameController structs & enums
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/** Enum representing all possible states a GameController button can be in. */
enum class Button : uint8_t {
	NOT_PRESSED,
	DOWN,
	HELD,
	UP
};

/** Class representing the state of a GameController at a given point in time. */
class GameController final {
public:
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline SDL_GameController* gameControllerPtr() const noexcept { return mGameControllerPtr; }
	inline int32_t id() const noexcept { return mID; } // Unique static identifier

	Button a = Button::NOT_PRESSED;
	Button b = Button::NOT_PRESSED;
	Button x = Button::NOT_PRESSED;
	Button y = Button::NOT_PRESSED;

	float stickDeadzone = 0.15f;
	float triggerDeadzone = 0.05f;

	vec2 leftStick; // Approximate range (length of vector): [0.0f, 1.0f]
	vec2 rightStick; // Approximate range (length of vector): [0.0f, 1.0f]
	Button leftStickButton = Button::NOT_PRESSED;
	Button rightStickButton = Button::NOT_PRESSED;

	Button leftShoulder = Button::NOT_PRESSED;
	Button rightShoulder = Button::NOT_PRESSED;
	float leftTrigger; // Range: (not-pressed) [0.0f, 1.0f] (fully-pressed)
	float rightTrigger; // Range: (not-pressed) [0.0f, 1.0f] (fully-pressed)

	Button padUp = Button::NOT_PRESSED;
	Button padDown = Button::NOT_PRESSED;
	Button padLeft = Button::NOT_PRESSED;
	Button padRight = Button::NOT_PRESSED;

	Button start = Button::NOT_PRESSED;
	Button back = Button::NOT_PRESSED;
	Button guide = Button::NOT_PRESSED;

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

	friend void update(vector<GameController>& controllers, const vector<SDL_Event>& events);

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	SDL_GameController* mGameControllerPtr;
	int32_t mID; // The SDL joystick id of this controller, used to identify.
};

// Update functions to update GameController struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void update(unordered_map<int32_t,GameController>& controllers, const vector<SDL_Event>& events) noexcept;

/** Starts the update process, should be called once before updateEvent() each frame. */
void updateStart(GameController& controller) noexcept;

/** Updates state with an SDL Event, should be called once for each event after updateStart(). */
void updateProcessEvent(GameController& controller, const SDL_Event& event) noexcept;

/** Finishes the update process, should be called once after all events have been processed. */
void updateFinish(GameController& controller) noexcept;

} // namespace sdl
#endif