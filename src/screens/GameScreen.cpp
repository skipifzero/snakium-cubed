#include "screens/GameScreen.hpp"

#include <sfz/geometry/AABB2D.hpp>

#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>
#include <sfz/gl/Scaler.hpp>

#include "GameLogic.hpp"
#include "GlobalConfig.hpp"
#include "Rendering.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/NewHighScoreScreen.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const std::string& basePath() noexcept
{
	static const std::string BASE_PATH{SDL_GetBasePath()};
	return BASE_PATH;
}

static const std::string& assetsPath() noexcept
{
	static const std::string ASSETS_PATH{basePath() + "assets/"};
	return ASSETS_PATH;
}

static const float TIME_UNTIL_GAME_OVER_SCREEN = 2.5f;

static void updateInputBuffer(Model& model, Camera& cam, DirectionInput* inputBufferPtr,
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
	mModel{modelCfg},

	mShortTermPerfStats{20}, 
	mLongerTermPerfStats{120},
	mLongestTermPerfStats{960},

	mPauseSystem{sfz::AABB2D{MENU_SYSTEM_DIM / 2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;

	const float buttonWidth = MENU_DIM.x * 0.35f;
	const int32_t musicVolume = GlobalConfig::INSTANCE().musicVolume;

	mPauseSystem.addSpacing(MENU_SYSTEM_DIM.y * 0.25f);
	addTitle(mPauseSystem, new TextItem{"Paused"});
	addStandardPadding(mPauseSystem);
	addHeading1(mPauseSystem, new Button{"Continue", [this](Button&) {
		this->mIsPaused = false;
		for (const auto& item : this->mPauseSystem.items()) {
			item->deselect();
		}
		Mix_ResumeMusic();
	}}, buttonWidth);
	addStandardPadding(mPauseSystem);

	// TODO: Remove later
	addHeading1(mPauseSystem, new Button{"Reload audio", [this](Button&) {
		Assets& assets = Assets::INSTANCE();

		assets.GAME_OVER_SFX = SoundEffect{(assetsPath() + "audio/sfx/game_over.wav").c_str()};
		assets.SHIFT_INITIATED_SFX = SoundEffect{(assetsPath() + "audio/sfx/shift_initiated.wav").c_str()};
		
		assets.OBJECT_EATEN_LATE_SFX = SoundEffect{(assetsPath() + "audio/sfx/object_eaten_late.wav").c_str()};
		assets.OBJECT_EATEN_LATE_SHIFT_SFX = SoundEffect{(assetsPath() + "audio/sfx/object_eaten_late_shift.wav").c_str()};
		assets.OBJECT_EATEN_SFX = SoundEffect{(assetsPath() + "audio/sfx/object_eaten.wav").c_str()};
		assets.OBJECT_EATEN_SHIFT_SFX = SoundEffect{(assetsPath() + "audio/sfx/object_eaten_shift.wav").c_str()};
		
		assets.BONUS_OBJECT_ADDED_SFX = SoundEffect{(assetsPath() + "audio/sfx/bonus_object_added.wav").c_str()};
		assets.BONUS_OBJECT_EATEN_SFX = SoundEffect{(assetsPath() + "audio/sfx/bonus_object_eaten.wav").c_str()};
		assets.BONUS_OBJECT_EATEN_SHIFT_SFX = SoundEffect{(assetsPath() + "audio/sfx/bonus_object_eaten_shift.wav").c_str()};
		assets.BONUS_OBJECT_MISSED_SFX = SoundEffect{(assetsPath() + "audio/sfx/bonus_object_missed.wav").c_str()};
	
	}}, buttonWidth);
	addStandardPadding(mPauseSystem);

	addHeading1(mPauseSystem, new Button{"Quit", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                  shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}, buttonWidth);

	
	if (musicVolume > 0) {
		Mix_VolumeMusic(int32_t(std::round(musicVolume * 12.8f)));
		Assets::INSTANCE().GAME_MUSIC.play();
	}
}

GameScreen::~GameScreen() noexcept
{
	sdl::stopMusic(200);
}

// GameScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp GameScreen::update(UpdateState& state)
{
	GlobalConfig& cfg = GlobalConfig::INSTANCE();
	Assets& assets = Assets::INSTANCE();

	mShortTermPerfStats.addSample(state.delta);
	mLongerTermPerfStats.addSample(state.delta);
	mLongestTermPerfStats.addSample(state.delta);

	// Small dive hack
	if (mCam.delayModelUpdate()) mInputBufferIndex = 0;

	// Handle input
	for (const SDL_Event& event : state.events) {
		if (!mIsPaused) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_SPACE:
					updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::SHIFT);
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
				case SDLK_F1:
					cfg.printFrametimes = !cfg.printFrametimes;
					break;
				case SDLK_F2:
					mUseModernRenderer = !mUseModernRenderer;
					break;

				case SDLK_ESCAPE:
					if (mModel.isGameOver()) {
						return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
						                std::shared_ptr<sfz::BaseScreen>{new NewHighScoreScreen{mModel.config(), mModel.stats()}}};
					} else {
						mIsPaused = true;
						Mix_PauseMusic();
						for (const auto& item : mPauseSystem.items()) {
							item->deselect();
						}
					}
				}
				break;
			}
		}
		else {
			if (event.type == SDL_KEYUP) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					mIsPaused = false;
					Mix_ResumeMusic();
				}
			}
		}
	}

	// Controller input
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);
	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (!mIsPaused) {
		if (ctrlId != -1) {
			const auto& ctrl = state.controllers[ctrlId];

			vec2 stick = ctrl.leftStick;
			float stickLen = length(stick);
			if (std::isnan(stickLen)) stickLen = 0.0f; // :v
			if (stickLen > 0.4f) {
				float upProj = dot(stick, vec2{0.0f, 1.0f});
				float downProj = dot(stick, vec2{0.0f, -1.0f});
				float leftProj = dot(stick, vec2{-1.0f, 0.0f});
				float rightProj = dot(stick, vec2{1.0f, 0.0f});
				float largestProj = std::max(upProj, std::max(downProj, std::max(leftProj, rightProj)));

				if (largestProj == upProj) {
					updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::UP);
				}
				else if (largestProj == downProj) {
					updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::DOWN);
				}
				else if (largestProj == leftProj) {
					updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::LEFT);
				}
				else if (largestProj == rightProj) {
					updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::RIGHT);
				}
			}


			if (ctrl.padUp == sdl::ButtonState::UP) {
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::UP);
			} else if (ctrl.padDown == sdl::ButtonState::UP) {
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::DOWN);
			} else if (ctrl.padLeft == sdl::ButtonState::UP) {
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::LEFT);
			} else if (ctrl.padRight == sdl::ButtonState::UP) {
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::RIGHT);
			} else if (ctrl.a == sdl::ButtonState::UP) {
				updateInputBuffer(mModel, mCam, mInputBuffer, 5, mInputBufferIndex, DirectionInput::SHIFT);
			} else if (ctrl.start == sdl::ButtonState::UP) {
				mIsPaused = true;
				Mix_PauseMusic();
				for (const auto& item : mPauseSystem.items()) {
					item->deselect();
				}
			}

		}
	}
	else {
		if (ctrlId != -1) {
			const auto& ctrl = state.controllers[ctrlId];
			if (ctrl.start == sdl::ButtonState::UP) {
				mIsPaused = false;
				Mix_ResumeMusic();
			}
		}
		mPauseSystem.update(data, state.delta);
		return mUpdateOp;
	}

	// Game over updating
	if (mModel.isGameOver()) {
		if (mTimeSinceGameOver >= TIME_UNTIL_GAME_OVER_SCREEN) {
			return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			                std::shared_ptr<sfz::BaseScreen>{new NewHighScoreScreen{mModel.config(), mModel.stats()}}};
		}
		mTimeSinceGameOver += state.delta;
	}

	if (mInputBufferIndex > 0) mModel.changeDirection(mCam.upDir(), mInputBuffer[0]);
	if (!mCam.delayModelUpdate()) mModel.update(state.delta);

	// Handle model events
	Event event = mModel.popEvent();
	while (event != Event::NONE) {
		if (event == Event::STATE_CHANGE) {
			if (mInputBufferIndex > 0) {
				mInputBufferIndex -= 1;
				for (size_t i = 0; i < (cfg.inputBufferSize-1); ++i) {
					mInputBuffer[i] = mInputBuffer[i+1];
				}
			}
		}
		else if (event == Event::GAME_OVER) {
			sdl::stopMusic(150);
			if (cfg.sfxVolume > 0) {
				Mix_Volume(-1, int32_t(std::round(cfg.sfxVolume * 12.8f)));
				assets.GAME_OVER_SFX.play();
			}
		}
		else if (cfg.sfxVolume > 0) {
			Mix_Volume(-1, int32_t(std::round(cfg.sfxVolume * 12.8f)));
			switch (event) {
			case Event::SHIFT_INITIATED:
				assets.SHIFT_INITIATED_SFX.play();
				break;
			case Event::OBJECT_EATEN_LATE:
				assets.OBJECT_EATEN_LATE_SFX.play();
				break;
			case Event::OBJECT_EATEN_LATE_SHIFT:
				assets.OBJECT_EATEN_LATE_SHIFT_SFX.play();
				break;
			case Event::OBJECT_EATEN:
				assets.OBJECT_EATEN_SFX.play();
				break;
			case Event::OBJECT_EATEN_SHIFT:
				assets.OBJECT_EATEN_SHIFT_SFX.play();
				break;
			case Event::BONUS_OBJECT_ADDED:
				assets.BONUS_OBJECT_ADDED_SFX.play();
				break;
			case Event::BONUS_OBJECT_EATEN:
				assets.BONUS_OBJECT_EATEN_SFX.play();
				break;
			case Event::BONUS_OBJECT_EATEN_SHIFT:
				assets.BONUS_OBJECT_EATEN_SHIFT_SFX.play();
				break;
			case Event::BONUS_OBJECT_MISSED:
				assets.BONUS_OBJECT_MISSED_SFX.play();
				break;
			default:
				sfz_assert_debug(false);
			}
		}
		event = mModel.popEvent();
	}

	mCam.onResize(60.0f, (float)state.window.drawableWidth()/(float)state.window.drawableHeight());
	mCam.update(mModel, state.delta);

	return mUpdateOp;
}

void GameScreen::render(UpdateState& state)
{
	GlobalConfig& cfg = GlobalConfig::INSTANCE();

	if (mUseModernRenderer) {
		mModernRenderer.render(mModel, mCam, state.window.drawableDimensions(), state.delta);
	} else {
		mClassicRenderer.render(mModel, mCam, AABB2D{state.window.drawableDimensions()/2.0f, state.window.drawableDimensions()});
	}

	vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, drawableDim.x, drawableDim.y);

	// Render text
	gl::FontRenderer& font = Assets::INSTANCE().fontRenderer;

	font.verticalAlign(gl::VerticalAlign::TOP);
	font.horizontalAlign(gl::HorizontalAlign::CENTER);

	char scoreBuffer[128];
	std::snprintf(scoreBuffer, 128, "Score: %i", totalScore(mModel.stats(), mModel.config()));

	const float size = 8.0f;
	const vec2 bgOffs = vec2{0.02f, -0.02f} * size;

	font.begin(guiCam);
	font.write(vec2{MENU_DIM.x / 2.0f, MENU_DIM.y} + bgOffs, size, scoreBuffer);
	font.write(vec2{MENU_DIM.x / 2.0f, MENU_DIM.y} - bgOffs, size, scoreBuffer);
	font.end(0, drawableDim, vec4{0.0f, 0.0f, 0.0f, 1.0f});

	font.begin(guiCam);
	font.write(vec2{MENU_DIM.x / 2.0f, MENU_DIM.y}, size, scoreBuffer);
	font.end(0, drawableDim, vec4{0.84f, 1.0f, 0.84f, 1.0f});

	if (mModel.isGameOver()) {

		font.verticalAlign(gl::VerticalAlign::MIDDLE);
		font.horizontalAlign(gl::HorizontalAlign::CENTER);

		font.begin(guiCam);
		font.write((MENU_DIM / 2.0f) + bgOffs, 20.0f, "Game Over");
		font.write((MENU_DIM / 2.0f) - bgOffs, 20.0f, "Game Over");
		font.end(0, drawableDim, vec4{0.0f, 0.0f, 0.0f, 1.0f});

		font.begin(guiCam);
		font.write((MENU_DIM / 2.0f), 20.0f, "Game Over");
		font.end(0, drawableDim, vec4{0.84f, 1.0f, 0.84f, 1.0f});
	}

	if (mIsPaused) {
		mPauseSystem.draw(0, drawableDim, guiCam);
	}

	// Render frametime stats
	if (cfg.printFrametimes) {
		char shortTermPerfBuffer[128];
		std::snprintf(shortTermPerfBuffer, 128, "Last %i frames: %s", mShortTermPerfStats.currentNumSamples(), mShortTermPerfStats.to_string());
		char longerTermPerfBuffer[128];
		std::snprintf(longerTermPerfBuffer, 128, "Last %i frames: %s", mLongerTermPerfStats.currentNumSamples(), mLongerTermPerfStats.to_string());
		char longestTermPerfBuffer[128];
		std::snprintf(longestTermPerfBuffer, 128, "Last %i frames: %s", mLongestTermPerfStats.currentNumSamples(), mLongestTermPerfStats.to_string());

		float fontSize = state.window.drawableHeight()/32.0f;
		float offset = fontSize*0.04f;
		float bottomOffset = state.window.drawableHeight()/128.0f;

		font.verticalAlign(gl::VerticalAlign::BOTTOM);
		font.horizontalAlign(gl::HorizontalAlign::LEFT);

		font.begin(state.window.drawableDimensions()/2.0f, state.window.drawableDimensions());
		font.write(vec2{offset, bottomOffset + fontSize*2.10f - offset}, fontSize, shortTermPerfBuffer);
		font.write(vec2{offset, bottomOffset + fontSize*1.05f - offset}, fontSize, longerTermPerfBuffer);
		font.write(vec2{offset, bottomOffset - offset}, fontSize, longestTermPerfBuffer);
		font.end(0, state.window.drawableDimensions(), sfz::vec4{0.0f, 0.0f, 0.0f, 1.0f});

		font.begin(state.window.drawableDimensions()/2.0f, state.window.drawableDimensions());
		font.write(vec2{0.0f, bottomOffset + fontSize*2.10f}, fontSize, shortTermPerfBuffer);
		font.write(vec2{0.0f, bottomOffset + fontSize*1.05f}, fontSize, longerTermPerfBuffer);
		font.write(vec2{0.0f, bottomOffset}, fontSize, longestTermPerfBuffer);
		font.end(0, state.window.drawableDimensions(), sfz::vec4{1.0f, 1.0f, 1.0f, 1.0f});
	}

	// Clean up
	glUseProgram(0);
}

} // namespace s3