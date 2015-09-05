#pragma once
#ifndef SFZ_SCREENS_BASE_SCREEN_HPP
#define SFZ_SCREENS_BASE_SCREEN_HPP

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "sfz/sdl/GameController.hpp"
#include "sfz/sdl/Mouse.hpp"
#include "sfz/sdl/Window.hpp"
#include "sfz/math/Vector.hpp"

namespace sfz {

using std::int32_t;
using std::shared_ptr;
using std::unordered_map;
using std::vector;

class BaseScreen; // Forward declaration for ScreenUpdateOp

// UpdateOp
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

enum class UpdateOpType {
	NO_OP,
	SWITCH_SCREEN,
	QUIT,
	REINIT_CONTROLLERS
};

struct UpdateOp final {
	UpdateOp() noexcept = default;
	UpdateOp(const UpdateOp&) noexcept = default;
	UpdateOp& operator= (const UpdateOp&) noexcept = default;
	inline UpdateOp(UpdateOpType type, shared_ptr<BaseScreen> screen = nullptr) noexcept
	:
		type{type},
		newScreen{screen}
	{ }

	UpdateOpType type;
	shared_ptr<BaseScreen> newScreen;
};

const UpdateOp SCREEN_NO_OP{UpdateOpType::NO_OP};
const UpdateOp SCREEN_QUIT{UpdateOpType::QUIT};
const UpdateOp SCREEN_REINIT_CONTROLLERS{UpdateOpType::REINIT_CONTROLLERS};

// UpdateState
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct UpdateState final {
	UpdateState() = delete;
	UpdateState(const UpdateState&) = delete;
	UpdateState& operator= (const UpdateState&) = delete;
	inline UpdateState(sdl::Window& window) noexcept : window{window} { }

	sdl::Window& window;
	vector<SDL_Event> events;
	vector<SDL_Event> controllerEvents;
	vector<SDL_Event> mouseEvents;
	unordered_map<int32_t, sdl::GameController> controllers;
	unordered_map<int32_t, sdl::GameControllerState> controllersLastFrameState;
	sdl::Mouse rawMouse;
	float delta;
};

// BaseScreen
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class BaseScreen {
public:
	virtual ~BaseScreen() = default;

	virtual UpdateOp update(const UpdateState& state) = 0;
	virtual void render(const UpdateState& state) = 0;

	virtual void onQuit();
	virtual void onResize(vec2 windowDimensions, vec2 drawableDimensions);
};

inline void BaseScreen::onQuit() { /* Default empty implementation. */ }
inline void BaseScreen::onResize(vec2, vec2) { /* Default empty implementation. */ }

} // namespace sfz

#endif