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
				mIsPaused = !mIsPaused;
				break;
			case SDLK_UP:
			case 'w':
			case 'W':
				mModel.changeDirection(mCam.mUpDir, Direction2D::UP);
				break;
			case SDLK_DOWN:
			case 's':
			case 'S':
				mModel.changeDirection(mCam.mUpDir, Direction2D::DOWN);
				break;
			case SDLK_LEFT:
			case 'a':
			case 'A':
				mModel.changeDirection(mCam.mUpDir, Direction2D::LEFT);
				break;
			case SDLK_RIGHT:
			case 'd':
			case 'D':
				mModel.changeDirection(mCam.mUpDir, Direction2D::RIGHT);
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
				          std::shared_ptr<sfz::BaseScreen>{new MainMenuScreen{}}};
			}
			break;
		}
	}

	// Updating
	if (mIsPaused) return sfz::SCREEN_NO_OP;

	mModel.update(state.delta);
	if (!mModel.mGameOver) mCam.update(mModel, state.delta);

	return sfz::SCREEN_NO_OP;
}

void GameScreen::render(UpdateState& state)
{
	mClassicRenderer.render(mModel, mCam, AABB2D{state.window.drawableDimensions()/2.0f, state.window.drawableDimensions()});
}

} // namespace s3