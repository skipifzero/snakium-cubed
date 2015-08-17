#include "screens/MainMenuScreen.hpp"

namespace s3 {

// MainMenuScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MainMenuScreen::MainMenuScreen(sdl::Window& window, Assets& assets) noexcept
:
	mWindow{window},
	mAssets{assets}
{ }

// MainMenuScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ScreenUpdateOp MainMenuScreen::update(const vector<SDL_Event>& events,
	                              const unordered_map<int32_t, sdl::GameController>& controllers,
	                              float delta)
{

	// Handle input
	for (const SDL_Event& event : events) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE: return ScreenUpdateOp{sfz::ScreenUpdateOpType::QUIT_APPLICATION};
			}
			break;
		case SDL_KEYUP:
			return ScreenUpdateOp{sfz::ScreenUpdateOpType::SWITCH_SCREEN, shared_ptr<BaseScreen>{new GameScreen{mWindow, mAssets, mCfg.modelConfig}}};
		}
	}

	return ScreenUpdateOp{sfz::ScreenUpdateOpType::NO_OPERATION};
}

void MainMenuScreen::render(float delta)
{

}

void MainMenuScreen::onQuit()
{
}

void MainMenuScreen::onResize(vec2 dimensions)
{
}

} // namespace s3