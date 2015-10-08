#include "screens/GameScreen.hpp"

#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>

#include "GameLogic.hpp"
#include "GlobalConfig.hpp"
#include "Rendering.hpp"
#include "screens/MainMenuScreen.hpp"

namespace s3 {

// GameScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GameScreen::GameScreen(const ModelConfig& modelCfg) noexcept
:
	mModel{modelCfg}
{ }

// GameScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp GameScreen::update(UpdateState& state)
{
	// Handle input
	for (const SDL_Event& event : state.events) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				mModel.changeDirection(mCam.upDir(), DirectionInput::DIVE);
				break;
			case SDLK_UP:
			case 'w':
			case 'W':
				mModel.changeDirection(mCam.upDir(), DirectionInput::UP);
				break;
			case SDLK_DOWN:
			case 's':
			case 'S':
				mModel.changeDirection(mCam.upDir(), DirectionInput::DOWN);
				break;
			case SDLK_LEFT:
			case 'a':
			case 'A':
				mModel.changeDirection(mCam.upDir(), DirectionInput::LEFT);
				break;
			case SDLK_RIGHT:
			case 'd':
			case 'D':
				mModel.changeDirection(mCam.upDir(), DirectionInput::RIGHT);
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case 'p':
			case 'P':
				mIsPaused = !mIsPaused;
				break;
			case 'r':
			case 'R':
				mUseModernRenderer = !mUseModernRenderer;
				break;
			case SDLK_ESCAPE:
				return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
				          std::shared_ptr<sfz::BaseScreen>{new MainMenuScreen{}}};
			}
			break;
		}
	}

	// Updating
	if (mIsPaused) return sfz::SCREEN_NO_OP;

	if (!mCam.delayModelUpdate()) mModel.update(state.delta);
	mCam.onResize(60.0f, (float)state.window.drawableWidth()/(float)state.window.drawableHeight());
	mCam.update(mModel, state.delta);

	return sfz::SCREEN_NO_OP;
}

void GameScreen::render(UpdateState& state)
{
	if (mUseModernRenderer) {
		mModernRenderer.render(mModel, mCam, AABB2D{state.window.drawableDimensions()/2.0f, state.window.drawableDimensions()});
	} else {
		mClassicRenderer.render(mModel, mCam, AABB2D{state.window.drawableDimensions()/2.0f, state.window.drawableDimensions()});
	}
}

} // namespace s3