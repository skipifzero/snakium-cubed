#include "screens/HighScoreScreen.hpp"

#include "sfz/gl/OpenGL.hpp"

#include "rendering/Assets.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/ResultScreen.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

using std::shared_ptr;

// HighScoreScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

HighScoreScreen::HighScoreScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;
	auto& a = Assets::INSTANCE();

	bool hasScores = loadScores(mScores);
	const float buttonWidth = MENU_DIM.x * 0.4f;
	float restPadding;
	if (hasScores) restPadding = calcRestPadding(3.0f, 0.0f, 9.0f, 4.0f);
	else restPadding = calcRestPadding(0.0f, 0.0f, 2.0f, 1.0f);

	const float scoreStrRatio = 0.3f;
	const float scoreButtonRatio = 0.3f;
	const float scoreNameRatio = 1.0f - scoreStrRatio - scoreButtonRatio;
	const float scoreStrWidth = 0.5f * mGuiSystem.bounds().width() * scoreStrRatio;
	const float scoreNameWidth = 0.85f * mGuiSystem.bounds().width() * scoreNameRatio;
	const float scoreButtonWidth = 0.5f * mGuiSystem.bounds().width() * scoreButtonRatio;

	// Title
	addTitle(mGuiSystem, new TextItem{"High Scores"});
	addStandardPadding(mGuiSystem);
	
	// Error text if high scores couldn't be loaded
	if (!hasScores) {
		addHeading3(mGuiSystem, new TextItem{"Could not load high scores file"});
		addHeading3(mGuiSystem, new TextItem{"(this is normal if no high scores has been made)"});
		addNavbar(mGuiSystem, new Button{"Back", [this](Button&) {
			this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
		}}, restPadding, buttonWidth);
		return;
	}

	addHeading1(mGuiSystem, new TextItem{"Standard"});
	for (size_t i = 0; i < NUM_SCORES_SAVED; ++i) {
		ThreeSplitContainer* tsc = new ThreeSplitContainer{scoreStrRatio, scoreButtonRatio};
		addHeading3(mGuiSystem, tsc);
		tsc->setLeft(new TextItem{std::to_string(totalScore(mScores.standardResults[i], STANDARD_CONFIG)), HorizontalAlign::LEFT}, scoreStrWidth, HorizontalAlign::RIGHT);
		tsc->setMiddle(new TextItem{mScores.standardNames[i], HorizontalAlign::LEFT}, scoreNameWidth);
		tsc->setRight(new Button{"Details", [this, i](Button&) {
			this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			                           shared_ptr<BaseScreen>{new ResultScreen{STANDARD_CONFIG, this->mScores.standardResults[i], true}}};
		}}, scoreButtonWidth, HorizontalAlign::LEFT);
		if (i >= mScores.numStandardResults) tsc->rightItem->disable();
	}
	addStandardPadding(mGuiSystem);
	
	addHeading1(mGuiSystem, new TextItem{"Large"});
	for (size_t i = 0; i < NUM_SCORES_SAVED; ++i) {
		ThreeSplitContainer* tsc = new ThreeSplitContainer{scoreStrRatio, scoreButtonRatio};
		addHeading3(mGuiSystem, tsc);
		tsc->setLeft(new TextItem{std::to_string(totalScore(mScores.largeResults[i], LARGE_CONFIG)), HorizontalAlign::LEFT}, scoreStrWidth, HorizontalAlign::RIGHT);
		tsc->setMiddle(new TextItem{mScores.largeNames[i], HorizontalAlign::LEFT}, scoreNameWidth);
		tsc->setRight(new Button{"Details", [this, i](Button&) {
			this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			                           shared_ptr<BaseScreen>{new ResultScreen{LARGE_CONFIG, this->mScores.largeResults[i], true}}};
		}}, scoreButtonWidth, HorizontalAlign::LEFT);
		if (i >= mScores.numLargeResults) tsc->rightItem->disable();
	}
	addStandardPadding(mGuiSystem);

	addHeading1(mGuiSystem, new TextItem{"Giant"});
	for (size_t i = 0; i < NUM_SCORES_SAVED; ++i) {
		ThreeSplitContainer* tsc = new ThreeSplitContainer{scoreStrRatio, scoreButtonRatio};
		addHeading3(mGuiSystem, tsc);
		tsc->setLeft(new TextItem{std::to_string(totalScore(mScores.giantResults[i], STANDARD_CONFIG)), HorizontalAlign::LEFT}, scoreStrWidth, HorizontalAlign::RIGHT);
		tsc->setMiddle(new TextItem{mScores.giantNames[i], HorizontalAlign::LEFT}, scoreNameWidth);
		tsc->setRight(new Button{"Details", [this, i](Button&) {
			this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			                           shared_ptr<BaseScreen>{new ResultScreen{GIANT_CONFIG, this->mScores.giantResults[i], true}}};
		}}, scoreButtonWidth, HorizontalAlign::LEFT);
		if (i >= mScores.numGiantResults) tsc->rightItem->disable();
	}
	addStandardPadding(mGuiSystem);

	// Navbar
	addNavbar(mGuiSystem, new Button{"Back", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}, restPadding, buttonWidth);
}

// HighScoreScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp HighScoreScreen::update(UpdateState& state)
{
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (cancelRef) {
		return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}
	mGuiSystem.update(data, state.delta);

	return mUpdateOp;
}

void HighScoreScreen::render(UpdateState& state)
{
	// Clearing screen
	glClearColor(MENU_BG_COLOR.x, MENU_BG_COLOR.y, MENU_BG_COLOR.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Sizes
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

	// Draw GUI
	mGuiSystem.draw(0, drawableDim, guiCam);
}

} // namespace s3