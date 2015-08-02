#include "sfz/sdl/GameController.hpp"

#include <sfz/Assert.hpp>

namespace sdl {

// Anonymous functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static Button* buttonPtr(GameController& c, uint8_t sdlButton)
{
	switch (static_cast<SDL_GameControllerButton>(sdlButton)) {
	case SDL_CONTROLLER_BUTTON_A: return &c.a;
	case SDL_CONTROLLER_BUTTON_B: return &c.b;
	case SDL_CONTROLLER_BUTTON_X: return &c.x;
	case SDL_CONTROLLER_BUTTON_Y: return &c.y;
	
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return &c.leftShoulder;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return &c.rightShoulder;
	case SDL_CONTROLLER_BUTTON_LEFTSTICK: return &c.leftStickButton;
	case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return &c.rightStickButton;

	case SDL_CONTROLLER_BUTTON_DPAD_UP: return &c.padUp;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return &c.padDown;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return &c.padLeft;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return &c.padRight;

	case SDL_CONTROLLER_BUTTON_START: return &c.start;
	case SDL_CONTROLLER_BUTTON_BACK: return &c.back;
	case SDL_CONTROLLER_BUTTON_GUIDE: return &c.guide;
	default: return nullptr;
	}
}

static SDL_GameController* openGameController(int deviceIndex) noexcept
{
	if (!SDL_IsGameController(deviceIndex)) {
		std::cerr << "deviceIndex: " << deviceIndex << " is not a GameController." << std::endl;
		return nullptr;
	}

	SDL_GameController* ptr = SDL_GameControllerOpen(deviceIndex);
	if (ptr == NULL) {
		std::cerr << "Could not open GameController at device index " << deviceIndex << ", error: "
		          << SDL_GetError() << std::endl;
		return nullptr;
	}

	return ptr;
}

static int32_t getJoystickId(SDL_GameController* ptr) noexcept
{
	if (ptr == nullptr) return -1;

	SDL_Joystick* joystick = SDL_GameControllerGetJoystick(ptr);
	if (joystick == NULL) {
		std::cerr << "Could not retrieve SDL_Joystick* from GameController, error: "
		          << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_JoystickID id = SDL_JoystickInstanceID(joystick);
	if (id < 0) {
		std::cerr << "Could not retrieve JoystickID from GameController, error: "
		          << SDL_GetError() << std::endl;
		return -1;
	}

	return id;
}

// GameController: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GameController::GameController() noexcept
:
	mGameControllerPtr{nullptr},
	mID{-1}
{ }

GameController::GameController(GameController&& other) noexcept
{
	this->mGameControllerPtr = other.mGameControllerPtr;
	this->mID = other.mID;
	other.mGameControllerPtr = nullptr;
	other.mID = -1;
}

GameController& GameController::operator= (GameController&& other) noexcept
{
	this->mGameControllerPtr = other.mGameControllerPtr;
	this->mID = other.mID;
	other.mGameControllerPtr = nullptr;
	other.mID = -1;
	return *this;
}

GameController::GameController(int deviceIndex) noexcept
:
	mGameControllerPtr{openGameController(deviceIndex)},
	mID{getJoystickId(mGameControllerPtr)}
{ }

GameController::~GameController() noexcept
{
	if (mGameControllerPtr != nullptr) {
		SDL_GameControllerClose(mGameControllerPtr);
	}
}

// Update functions to update GameController struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void update(unordered_map<int32_t,GameController>& controllers, const vector<SDL_Event>& events) noexcept
{
	for (auto& c : controllers) updateStart(std::get<1>(c));

	for (const SDL_Event& event : events) {
		switch (event.type) {
		case SDL_CONTROLLERDEVICEADDED:
			// which is the device index in this context
			{
				GameController c{event.cdevice.which};
				if (c.id() == -1) break;
				if (controllers.find(c.id()) != controllers.end()) break;
				controllers[c.id()] = std::move(c);
			}
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			// which is the joystick id in this context
			if (controllers.find(event.cdevice.which) != controllers.end()) {
				controllers.erase(event.cdevice.which);
			}
			break;
		case SDL_CONTROLLERDEVICEREMAPPED:
			// TODO: Nothing of value to do here?
			break;
			
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
			if (controllers.find(event.cbutton.which) != controllers.end()) {
				updateProcessEvent(controllers[event.cbutton.which], event);
			}
			break;
		case SDL_CONTROLLERAXISMOTION:
			if (controllers.find(event.caxis.which) != controllers.end()) {
				updateProcessEvent(controllers[event.caxis.which], event);
			}
			break;
		}
	}

	for (auto& c : controllers) updateFinish(std::get<1>(c));
}

void updateStart(GameController& c) noexcept
{
	// Changes previous DOWN state to HELD state.

	if (c.a == Button::DOWN) c.a = Button::HELD;
	if (c.b == Button::DOWN) c.b = Button::HELD;
	if (c.x == Button::DOWN) c.x = Button::HELD;
	if (c.y == Button::DOWN) c.y = Button::HELD;

	if (c.leftShoulder == Button::DOWN) c.leftShoulder = Button::HELD;
	if (c.rightShoulder == Button::DOWN) c.rightShoulder = Button::HELD;
	if (c.leftStickButton == Button::DOWN) c.leftStickButton = Button::HELD;
	if (c.rightStickButton == Button::DOWN) c.rightStickButton = Button::HELD;

	if (c.padUp == Button::DOWN) c.padUp = Button::HELD;
	if (c.padDown == Button::DOWN) c.padDown = Button::HELD;
	if (c.padLeft == Button::DOWN) c.padLeft = Button::HELD;
	if (c.padRight == Button::DOWN) c.padRight = Button::HELD;

	if (c.start == Button::DOWN) c.start = Button::HELD;
	if (c.back == Button::DOWN) c.back = Button::HELD;
	if (c.guide == Button::DOWN) c.guide = Button::HELD;

	// Changes previous UP state to NOT_PRESSED state.

	if (c.a == Button::UP) c.a = Button::NOT_PRESSED;
	if (c.b == Button::UP) c.b = Button::NOT_PRESSED;
	if (c.x == Button::UP) c.x = Button::NOT_PRESSED;
	if (c.y == Button::UP) c.y = Button::NOT_PRESSED;

	if (c.leftShoulder == Button::UP) c.leftShoulder = Button::NOT_PRESSED;
	if (c.rightShoulder == Button::UP) c.rightShoulder = Button::NOT_PRESSED;
	if (c.leftStickButton == Button::UP) c.leftStickButton = Button::NOT_PRESSED;
	if (c.rightStickButton == Button::UP) c.rightStickButton = Button::NOT_PRESSED;

	if (c.padUp == Button::UP) c.padUp = Button::NOT_PRESSED;
	if (c.padDown == Button::UP) c.padDown = Button::NOT_PRESSED;
	if (c.padLeft == Button::UP) c.padLeft = Button::NOT_PRESSED;
	if (c.padRight == Button::UP) c.padRight = Button::NOT_PRESSED;

	if (c.start == Button::UP) c.start = Button::NOT_PRESSED;
	if (c.back == Button::UP) c.back = Button::NOT_PRESSED;
	if (c.guide == Button::UP) c.guide = Button::NOT_PRESSED;
}

void updateProcessEvent(GameController& controller, const SDL_Event& event) noexcept
{
	const float AXIS_MAX = 32766.0f; // Actual range [-32768, 32767]
	const float TRIGGER_MAX = 32766.0f; // Actual range [0, 32767]

	Button* ptr = nullptr;
	switch (event.type) {
	case SDL_CONTROLLERBUTTONDOWN:
		/*std::cout << "Button down (" << event.cbutton.which << "): " << (int)event.cbutton.button << ", name: "
					  << SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event.cbutton.button)
					  << std::endl;*/
		ptr = buttonPtr(controller, event.cbutton.button);
		if (ptr != nullptr) *ptr = Button::DOWN;
		break;
	case SDL_CONTROLLERBUTTONUP:
		/*std::cout << "Button up (" << event.cbutton.which << "): " << (int)event.cbutton.button << ", name: "
					  << SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event.cbutton.button)
					  << std::endl;*/
		ptr = buttonPtr(controller, event.cbutton.button);
		if (ptr != nullptr) *ptr = Button::UP;
		break;
	case SDL_CONTROLLERAXISMOTION:
		/*std::cout << "Axis " << (int)event.caxis.axis << ", name: "
					  << SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)event.caxis.axis)
					  << ", value: " << event.caxis.value << std::endl;*/
		
		const float axisVal = static_cast<float>(event.caxis.value);		
		switch (static_cast<SDL_GameControllerAxis>(event.caxis.axis)) {
		case SDL_CONTROLLER_AXIS_LEFTX:
			controller.leftStick[0] = axisVal/AXIS_MAX;
			break;
		case SDL_CONTROLLER_AXIS_LEFTY:
			controller.leftStick[1] = -axisVal/AXIS_MAX;
			break;
		case SDL_CONTROLLER_AXIS_RIGHTX:
			controller.rightStick[0] = axisVal/AXIS_MAX;
			break;
		case SDL_CONTROLLER_AXIS_RIGHTY:
			controller.rightStick[1] = -axisVal/AXIS_MAX;
			break;
		case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
			controller.leftTrigger = axisVal/TRIGGER_MAX;
			break;
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
			controller.rightTrigger = axisVal/TRIGGER_MAX;
			break;
		case SDL_CONTROLLER_AXIS_INVALID:
			std::cerr << "event.caxis.axis == SDL_CONTROLLER_AXIS_INVALID" << std::endl;
			break;
		case SDL_CONTROLLER_AXIS_MAX:
			std::cerr << "event.caxis.axis == SDL_CONTROLLER_AXIS_MAX" << std::endl;
			break;
		} 
		break;
	}
}

void updateFinish(GameController& controller) noexcept
{
	// Deadzone checks
	if (sfz::length(controller.leftStick) < controller.stickDeadzone) {
		controller.leftStick[0] = 0.0f;
		controller.leftStick[1] = 0.0f;
	}
	if (sfz::length(controller.rightStick) < controller.stickDeadzone) {
		controller.rightStick[0] = 0.0f;
		controller.rightStick[1] = 0.0f;
	}
	if (controller.leftTrigger < controller.triggerDeadzone) {
		controller.leftTrigger = 0.0f;
	}
	if (controller.rightTrigger < controller.triggerDeadzone) {
		controller.rightTrigger = 0.0f;
	}

	// Normalize if norm is > 1
	if (sfz::length(controller.leftStick) > 1.0f) {
		controller.leftStick = sfz::normalize(controller.leftStick);
	}
	if (sfz::length(controller.rightStick) > 1.0f) {
		controller.rightStick = sfz::normalize(controller.rightStick);
	}
	if (controller.leftTrigger > 1.0f) {
		controller.leftTrigger = 1.0f;
	}
	if (controller.rightTrigger > 1.0f) {
		controller.rightTrigger = 1.0f;
	}
}

} // namespace sdl