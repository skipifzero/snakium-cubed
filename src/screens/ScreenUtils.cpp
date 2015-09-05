#include "screens/ScreenUtils.hpp"

namespace s3 {

int32_t getFirstController(const sfz::UpdateState& state) noexcept
{
	auto itr = state.controllers.begin();
	if (itr != state.controllers.end()) return itr->first;
	return -1;
}

gui::InputData inputDataFromUpdateState(const sfz::UpdateState& state,
                                        vec2 camPos, vec2 camDim, int32_t ctrlId) noexcept
{
	gui::InputData data;
	data.key = gui::KeyInput::NONE;

	// Check for keyboard events
	for (const SDL_Event& event : state.events) {
		switch (event.type) {
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_UP:
			case 'w':
			case 'W':
				data.key = gui::KeyInput::UP;
				break;
			case SDLK_DOWN:
			case 's':
			case 'S':
				data.key = gui::KeyInput::DOWN;
				break;
			case SDLK_LEFT:
			case 'a':
			case 'A':
				data.key = gui::KeyInput::LEFT;
				break;
			case SDLK_RIGHT:
			case 'd':
			case 'D':
				data.key = gui::KeyInput::DOWN;
				break;
			case SDLK_RETURN:
			case SDLK_SPACE:
				data.key = gui::KeyInput::ACTIVATE;
				break;
			}
		}
	}

	// Mouse
	auto scaledMouse = state.rawMouse.scaleMouse(camPos, camDim);
	data.pointerPos = scaledMouse.position;
	data.pointerMotion = scaledMouse.motion;
	data.pointerState = scaledMouse.leftButton;
	data.scrollWheel = scaledMouse.wheel;

	// Controller
	auto& ctrlItr = state.controllers.find(ctrlId);
	auto& lastCtrlItr = state.controllersLastFrameState.find(ctrlId);
	if (ctrlItr != state.controllers.end() && lastCtrlItr != state.controllersLastFrameState.end()) {
		auto& ctrl = ctrlItr->second;
		auto& lastCtrl = lastCtrlItr->second;

		// Left stick (pad emulation)
		if (sfz::length(ctrl.leftStick) > 0.8f && sfz::length(lastCtrl.leftStick) < 0.8f) {
			float leftAngle = sfz::angle(ctrl.leftStick);
			const float PIdiv4 = sfz::PI()/4.0f;
			if ((PIdiv4) < leftAngle && leftAngle < (3.0f*PIdiv4)) { // UP
				data.key = gui::KeyInput::UP;
			} else if ((3.0f*PIdiv4) < leftAngle && leftAngle < (5.0f*PIdiv4)) { // LEFT
				data.key = gui::KeyInput::LEFT;
			} else if ((5.0f*PIdiv4) < leftAngle && leftAngle < (7.0f*PIdiv4)) { // DOWN
				data.key = gui::KeyInput::DOWN;
			} else if (leftAngle < (PIdiv4) || (7.0f*PIdiv4) < leftAngle) { // RIGHT
				data.key = gui::KeyInput::RIGHT;
			}
		}

		/*// Right stick (scroll wheel emulation)
		if (data.scrollWheel == vec2{0.0f}) {
			data.scrollWheel = ctrl.rightStick;
		}*/

		if (ctrl.padUp == sdl::ButtonState::UP) {
			data.key = gui::KeyInput::UP;
		} else if (ctrl.padDown == sdl::ButtonState::UP) {
			data.key = gui::KeyInput::DOWN;
		} else if (ctrl.padLeft == sdl::ButtonState::UP) {
			data.key = gui::KeyInput::LEFT;
		} else if (ctrl.padRight == sdl::ButtonState::UP) {
			data.key = gui::KeyInput::RIGHT;
		} else if (ctrl.a == sdl::ButtonState::UP) {
			data.key = gui::KeyInput::ACTIVATE;
		}
	}

	return data;
}

} // namespace s3