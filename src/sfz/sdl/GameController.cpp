#include "sfz/sdl/GameController.hpp"

namespace sdl {

// Anonymous functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {

Button& buttonReference(GameController& c, uint8_t sdlButton)
{
	switch (static_cast<SDL_GameControllerButton>(sdlButton)) {
	case SDL_CONTROLLER_BUTTON_A: return c.mButtonA;
	case SDL_CONTROLLER_BUTTON_B: return c.mButtonB;
	case SDL_CONTROLLER_BUTTON_X: return c.mButtonX;
	case SDL_CONTROLLER_BUTTON_Y: return c.mButtonY;
	
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return c.mButtonLeftShoulder;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return c.mButtonRightShoulder;
	case SDL_CONTROLLER_BUTTON_LEFTSTICK: return c.mButtonLeftStick;
	case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return c.mButtonRightStick;

	case SDL_CONTROLLER_BUTTON_DPAD_UP: return c.mButtonDPADUp;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return c.mButtonDPADDown;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return c.mButtonDPADLeft;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return c.mButtonDPADRight;

	case SDL_CONTROLLER_BUTTON_START: return c.mButtonStart;
	case SDL_CONTROLLER_BUTTON_BACK: return c.mButtonBack;
	case SDL_CONTROLLER_BUTTON_GUIDE: return c.mButtonGuide;
	}
	sfz_assert_debug(false);
}

}

// Update functions to update GameController struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void updateStart(GameController& c) noexcept
{
	// Changes previous DOWN state to HELD state.

	if (c.mButtonA == Button::DOWN) c.mButtonA = Button::HELD;
	if (c.mButtonB == Button::DOWN) c.mButtonB = Button::HELD;
	if (c.mButtonX == Button::DOWN) c.mButtonX = Button::HELD;
	if (c.mButtonY == Button::DOWN) c.mButtonY = Button::HELD;

	if (c.mButtonLeftShoulder == Button::DOWN) c.mButtonLeftShoulder = Button::HELD;
	if (c.mButtonRightShoulder == Button::DOWN) c.mButtonRightShoulder = Button::HELD;
	if (c.mButtonLeftStick == Button::DOWN) c.mButtonLeftStick = Button::HELD;
	if (c.mButtonRightStick == Button::DOWN) c.mButtonRightStick = Button::HELD;

	if (c.mButtonDPADUp == Button::DOWN) c.mButtonDPADUp = Button::HELD;
	if (c.mButtonDPADDown == Button::DOWN) c.mButtonDPADDown = Button::HELD;
	if (c.mButtonDPADLeft == Button::DOWN) c.mButtonDPADLeft = Button::HELD;
	if (c.mButtonDPADRight == Button::DOWN) c.mButtonDPADRight = Button::HELD;

	if (c.mButtonStart == Button::DOWN) c.mButtonStart = Button::HELD;
	if (c.mButtonBack == Button::DOWN) c.mButtonBack = Button::HELD;
	if (c.mButtonGuide == Button::DOWN) c.mButtonGuide = Button::HELD;

	// Changes previous UP state to NOT_PRESSED state.

	if (c.mButtonA == Button::UP) c.mButtonA = Button::NOT_PRESSED;
	if (c.mButtonB == Button::UP) c.mButtonB = Button::NOT_PRESSED;
	if (c.mButtonX == Button::UP) c.mButtonX = Button::NOT_PRESSED;
	if (c.mButtonY == Button::UP) c.mButtonY = Button::NOT_PRESSED;

	if (c.mButtonLeftShoulder == Button::UP) c.mButtonLeftShoulder = Button::NOT_PRESSED;
	if (c.mButtonRightShoulder == Button::UP) c.mButtonRightShoulder = Button::NOT_PRESSED;
	if (c.mButtonLeftStick == Button::UP) c.mButtonLeftStick = Button::NOT_PRESSED;
	if (c.mButtonRightStick == Button::UP) c.mButtonRightStick = Button::NOT_PRESSED;

	if (c.mButtonDPADUp == Button::UP) c.mButtonDPADUp = Button::NOT_PRESSED;
	if (c.mButtonDPADDown == Button::UP) c.mButtonDPADDown = Button::NOT_PRESSED;
	if (c.mButtonDPADLeft == Button::UP) c.mButtonDPADLeft = Button::NOT_PRESSED;
	if (c.mButtonDPADRight == Button::UP) c.mButtonDPADRight = Button::NOT_PRESSED;

	if (c.mButtonStart == Button::UP) c.mButtonStart = Button::NOT_PRESSED;
	if (c.mButtonBack == Button::UP) c.mButtonBack = Button::NOT_PRESSED;
	if (c.mButtonGuide == Button::UP) c.mButtonGuide = Button::NOT_PRESSED;
}

void updateProcessEvent(GameController& controller, const SDL_Event& event) noexcept
{
	static const float AXIS_MAX = 32766.0f; // Actual range [-32768, 32767]
	static const float TRIGGER_MAX = 32766.0f; // Actual range [0, 32767]

	switch (event.type) {
	case SDL_CONTROLLERBUTTONDOWN:
		/*std::cout << "Button down (" << event.cbutton.which << "): " << (int)event.cbutton.button << ", name: "
					  << SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event.cbutton.button)
					  << std::endl;*/
		buttonReference(controller, event.cbutton.button) = Button::DOWN;
		break;
	case SDL_CONTROLLERBUTTONUP:
		/*std::cout << "Button up (" << event.cbutton.which << "): " << (int)event.cbutton.button << ", name: "
					  << SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event.cbutton.button)
					  << std::endl;*/
		buttonReference(controller, event.cbutton.button) = Button::UP;
		break;
	case SDL_CONTROLLERAXISMOTION:
		/*std::cout << "Axis " << (int)event.caxis.axis << ", name: "
					  << SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)event.caxis.axis)
					  << ", value: " << event.caxis.value << std::endl;*/
		
		const float axisVal = static_cast<float>(event.caxis.value);		
		switch (static_cast<SDL_GameControllerAxis>(event.caxis.axis)) {
		case SDL_CONTROLLER_AXIS_LEFTX:
			controller.mLeftStick[0] = axisVal/AXIS_MAX;
			break;
		case SDL_CONTROLLER_AXIS_LEFTY:
			controller.mLeftStick[1] = -axisVal/AXIS_MAX;
			break;
		case SDL_CONTROLLER_AXIS_RIGHTX:
			controller.mRightStick[0] = axisVal/AXIS_MAX;
			break;
		case SDL_CONTROLLER_AXIS_RIGHTY:
			controller.mRightStick[1] = -axisVal/AXIS_MAX;
			break;
		case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
			controller.mLeftTrigger = axisVal/TRIGGER_MAX;
			break;
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
			controller.mRightTrigger = axisVal/TRIGGER_MAX;
			break;
		} 
		break;
	}
}

void updateFinish(GameController& controller) noexcept
{
	// Deadzone checks
	if (sfz::length(controller.mLeftStick) < controller.mLeftStickDeadzone) {
		controller.mLeftStick[0] = 0.0f;
		controller.mLeftStick[1] = 0.0f;
	}
	if (sfz::length(controller.mRightStick) < controller.mRightStickDeadzone) {
		controller.mRightStick[0] = 0.0f;
		controller.mRightStick[1] = 0.0f;
	}
	if (controller.mLeftTrigger < controller.mLeftTriggerDeadzone) {
		controller.mLeftTrigger = 0.0f;
	}
	if (controller.mRightTrigger < controller.mRightTriggerDeadzone) {
		controller.mRightTrigger = 0.0f;
	}

	// Normalize if norm is > 1
	if (sfz::length(controller.mLeftStick) > 1.0f) {
		controller.mLeftStick = sfz::normalize(controller.mLeftStick);
	}
	if (sfz::length(controller.mRightStick) > 1.0f) {
		controller.mRightStick = sfz::normalize(controller.mRightStick);
	}
	if (controller.mLeftTrigger > 1.0f) {
		controller.mLeftTrigger = 1.0f;
	}
	if (controller.mRightTrigger > 1.0f) {
		controller.mRightTrigger = 1.0f;
	}
}

} // namespace sdl