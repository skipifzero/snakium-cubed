#include "sfz/screens/GameLoop.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "sfz/sdl/GameController.hpp"
#include "sfz/math/Vector.hpp"

namespace sfz {

// Typedefs
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

using std::int32_t;
using time_point = std::chrono::high_resolution_clock::time_point;
using std::unordered_map;
using std::vector;

// Static helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static float calculateDelta(time_point& previousTime) noexcept
{
	time_point currentTime = std::chrono::high_resolution_clock::now();

	using FloatSecond = std::chrono::duration<float>;
	float delta = std::chrono::duration_cast<FloatSecond>(currentTime - previousTime).count();

	previousTime = currentTime;
	return delta;
}

static void initControllers(unordered_map<int32_t, sdl::GameController>& controllers) noexcept
{
	controllers.clear();

	int numJoysticks = SDL_NumJoysticks();
	for (int i = 0; i < numJoysticks; ++i) {
		if (!SDL_IsGameController(i)) continue;
		
		sdl::GameController c{i};
		if (c.id() == -1) continue;
		if (controllers.find(c.id()) != controllers.end()) continue;

		controllers[c.id()] = std::move(c);
	}
}

// GameLoop function
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void runGameLoop(sdl::Window& window, shared_ptr<BaseScreen> currentScreen)
{
	// Initialize controllers
	unordered_map<int32_t, sdl::GameController> controllers;
	initControllers(controllers);

	// Initialize time delta
	time_point previousTime;
	float delta = calculateDelta(previousTime);

	// Initialize SDL events
	SDL_GameControllerEventState(SDL_ENABLE);
	vector<SDL_Event> events;
	vector<SDL_Event> controllerEvents;
	SDL_Event event;

	while (true) {
		// Calculate delta
		delta = calculateDelta(previousTime);
		delta = std::min(delta, 0.25f);

		// Process events
		events.clear();
		controllerEvents.clear();
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {

			// Quitting and resizing window
			case SDL_QUIT:
				currentScreen->onQuit();
				return;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					currentScreen->onResize(vec2{(float)event.window.data1,
					                             (float)event.window.data1});
					break;
				default:
					events.push_back(event);
					break;
				}
				break;

			// SDL_GameController events
			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
			case SDL_CONTROLLERDEVICEREMAPPED:
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERAXISMOTION:
				controllerEvents.push_back(event);
				break;

			default:
				events.push_back(event);
				break;
			}
		}

		// Updates controllers
		update(controllers, controllerEvents);

		// Update current screen
		ScreenUpdateOp op = currentScreen->update(events, controllers, delta);

		// Perform eventual operations requested by screen update
		switch (op.type) {
		case ScreenUpdateOpType::SWITCH_SCREEN:
			currentScreen = op.newScreen;
			continue;
		case ScreenUpdateOpType::QUIT_APPLICATION:
			currentScreen->onQuit();
			return;
		case ScreenUpdateOpType::REINITIALIZE_CONTROLLERS:
			initControllers(controllers);
			continue;

		case ScreenUpdateOpType::NO_OPERATION:
		default:
			// Do nothing
			break;
		}

		// Render current screen
		currentScreen->render(delta);

		SDL_GL_SwapWindow(window.mPtr);
	}
}

} // namespace sfz