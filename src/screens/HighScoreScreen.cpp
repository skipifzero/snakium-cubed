#include "screens/HighScoreScreen.hpp"

#include "sfz/gl/OpenGL.hpp"

#include "rendering/Assets.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/MenuConstants.hpp"
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
	if (hasScores) restPadding = calcRestPadding(1.0f, 5.0f, 1.0f, 0.0f);
	else restPadding = calcRestPadding(0.0f, 0.0f, 2.0f, 1.0f);

	// Title
	addTitle(mGuiSystem, new TextItem{"High Scores"});
	
	// Error text if high scores couldn't be loaded
	if (!hasScores) {
		addStandardPadding(mGuiSystem);
		addHeading3(mGuiSystem, new TextItem{"Could not load high scores file"});
		addHeading3(mGuiSystem, new TextItem{"(this is normal if no high scores has been made)"});
		addNavbar(mGuiSystem, new Button{"Back", [this](Button&) {
			this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
		}}, restPadding, buttonWidth);
		return;
	}

	mModeStr = new TextItem{"Standard"};
	addHeading1(mGuiSystem, mModeStr);

	for (size_t i = 0; i < NUM_SCORES_SAVED; ++i) {
		mScoreItems[i].scoreItem = new (std::nothrow) TextItem{std::to_string(totalScore(mScores.standardResults[i], STANDARD_CONFIG))};
		mScoreItems[i].nameItem = new (std::nothrow) TextItem{mScores.standardNames[i]};
		mScoreItems[i].detailsButton = new (std::nothrow) Button{">>", [](Button&) {
			
		}};
		ThreeSplitContainer* tsc = new (std::nothrow) ThreeSplitContainer{};
		addHeading2(mGuiSystem, tsc);
		tsc->setLeft(mScoreItems[i].scoreItem, tsc->dim.x / 4.0f);
		tsc->setMiddle(mScoreItems[i].nameItem, tsc->dim.x / 4.0f);
		tsc->setRight(mScoreItems[i].detailsButton, tsc->dim.x / 4.0f);
	}
	


	ThreeSplitContainer* tsc = new (std::nothrow) ThreeSplitContainer{};
	addHeading3(mGuiSystem, tsc);

	float tscWidth = tsc->dim.x / 4.0f;

	tsc->setLeft(new Button{"left", [](Button& b){
		b.disable();
	}}, tscWidth, HorizontalAlign::CENTER);

	tsc->setMiddle(new Button{"middle", [](Button& b) {
		b.disable();
	}}, tscWidth, HorizontalAlign::CENTER);

	tsc->setRight(new Button{"right", [](Button& b) {
		b.disable();
	}}, tscWidth, HorizontalAlign::CENTER);


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