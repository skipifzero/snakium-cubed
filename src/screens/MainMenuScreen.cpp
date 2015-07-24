#include "screens/MainMenuScreen.hpp"

namespace s3 {

// MainMenuScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MainMenuScreen::MainMenuScreen(sdl::Window& window, Assets& assets) noexcept
:
	mWindow{window},
	mAssets{assets}
{
	mCfg.load();
}

// MainMenuScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void MainMenuScreen::update(const std::vector<SDL_Event>& events, float delta)
{
	// Handle input
	for (const SDL_Event& event : events) {
		switch (event.type) {
		case SDL_QUIT:
			mQuit = true;
			return;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				mQuit = true;
				return;
			}
			break;
		case SDL_KEYUP:
			mChangeScreenPtr = new GameScreen{mWindow, mAssets, mCfg.mModelConfig};
			return;
		}
	}
}

IScreen* MainMenuScreen::changeScreen()
{
	return mChangeScreenPtr;
}

bool MainMenuScreen::quit()
{
	return mQuit;
}

void MainMenuScreen::render(float delta)
{

}


} // namespace s3