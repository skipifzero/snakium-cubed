#include "screens/GameScreen.hpp"

#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>

#include "GameLogic.hpp"
#include "GlobalConfig.hpp"
#include "Rendering.hpp"
#include "screens/MainMenuScreen.hpp"

namespace s3 {

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void updateInputBuffer(Model& model, Camera& cam, DirectionInput* inputBufferPtr,
                       size_t bufferSize, size_t& index, DirectionInput dirInput) noexcept
{
	GlobalConfig& cfg = GlobalConfig::INSTANCE();

	sfz_assert_debug(cfg.inputBufferSize <= bufferSize);
	sfz_assert_debug(index <= cfg.inputBufferSize);

	if (index == 0) {
		if (!model.isChangingDirection(cam.upDir(), dirInput)) {
			return;
		}
		inputBufferPtr[index] = dirInput;
		index += 1;
	}
	else if (index < cfg.inputBufferSize) {
		if (inputBufferPtr[index-1] == dirInput ||
		    inputBufferPtr[index-1] == opposite(dirInput)) {
			return;
		}
		inputBufferPtr[index] = dirInput;
		index += 1;
	}
	else {
		inputBufferPtr[cfg.inputBufferSize-1] = dirInput;
		index = cfg.inputBufferSize;
	}
}

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
	GlobalConfig& cfg = GlobalConfig::INSTANCE();

	// Small dive hack
	if (mCam.delayModelUpdate()) mInputBufferIndex = 0;

	// Handle input
	for (const SDL_Event& event : state.events) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::DIVE);
				break;
			case SDLK_UP:
			case 'w':
			case 'W':
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::UP);
				break;
			case SDLK_DOWN:
			case 's':
			case 'S':
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::DOWN);
				break;
			case SDLK_LEFT:
			case 'a':
			case 'A':
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::LEFT);
				break;
			case SDLK_RIGHT:
			case 'd':
			case 'D':
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::RIGHT);
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

	if (mInputBufferIndex > 0) mModel.changeDirection(mCam.upDir(), mInputBuffer[0]);
	bool changeOccured = false;
	if (!mCam.delayModelUpdate()) mModel.update(state.delta, &changeOccured);

	if (changeOccured && mInputBufferIndex > 0) {
		mInputBufferIndex -= 1;
		for (size_t i = 0; i < (cfg.inputBufferSize-1); ++i) {
			mInputBuffer[i] = mInputBuffer[i+1];
		}
	}

	mCam.onResize(60.0f, (float)state.window.drawableWidth()/(float)state.window.drawableHeight());
	mCam.update(mModel, state.delta);

	return sfz::SCREEN_NO_OP;
}

void GameScreen::render(UpdateState& state)
{
	if (mUseModernRenderer) {
		mModernRenderer.render(mModel, mCam, state.window.drawableDimensions());
	} else {
		mClassicRenderer.render(mModel, mCam, AABB2D{state.window.drawableDimensions()/2.0f, state.window.drawableDimensions()});
	}

	vec2 drawableDim = state.window.drawableDimensions();
	
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, drawableDim.x, drawableDim.y);

	// Render text
	gl::FontRenderer& font = Assets::INSTANCE().fontRenderer;

	font.verticalAlign(gl::VerticalAlign::TOP);
	font.horizontalAlign(gl::HorizontalAlign::LEFT);

	font.begin(drawableDim/2.0f, drawableDim);

	font.write(vec2{0.0f, drawableDim.y}, 64.0f, "Score: " + std::to_string(mModel.score()));

	font.end(0, drawableDim, vec4{1.0f, 1.0f, 1.0f, 1.0f});

	if (mModel.isGameOver()) {
		font.verticalAlign(gl::VerticalAlign::MIDDLE);
		font.horizontalAlign(gl::HorizontalAlign::CENTER);

		font.begin(drawableDim/2.0f, drawableDim);

		font.write(drawableDim/2.0f, 160.0f, "Game Over");

		font.end(0, drawableDim, sfz::vec4{1.0f, 1.0f, 1.0f, 1.0f});
	}

	// Clean up
	glUseProgram(0);
}

} // namespace s3