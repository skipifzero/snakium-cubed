#include "screens/MainMenuScreen.hpp"

#include <memory>

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "screens/HighScoreScreen.hpp"
#include "screens/ModeSelectScreen.hpp"
#include "screens/OptionsScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/RulesScreen.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

using std::shared_ptr;

// MainMenuScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MainMenuScreen::MainMenuScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;
	auto& a = Assets::INSTANCE();

	const float buttonWidth = MENU_DIM.x * 0.45f;
	const float restPadding = calcRestPadding(5.0f, 0.0f, 0.0f, 6.0f);

	addTitle(mGuiSystem, new ImageItem(a.SNAKIUM_LOGO));

	addStandardPadding(mGuiSystem);

	addHeading1(mGuiSystem, new Button{"New Game", [this](Button& ref) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new ModeSelectScreen()}};
	}}, buttonWidth);
	addStandardPadding(mGuiSystem);

	addHeading1(mGuiSystem, new Button{"High Scores", [this](Button& ref) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new HighScoreScreen()}};
	}}, buttonWidth);
	addStandardPadding(mGuiSystem);

	addHeading1(mGuiSystem, new Button{"Rules", [this](Button& ref) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new RulesScreen()}};
	}}, buttonWidth);
	addStandardPadding(mGuiSystem);

	addHeading1(mGuiSystem, new Button{"Options", [this](Button& ref) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new OptionsScreen{}}};
	}}, buttonWidth);
	addStandardPadding(mGuiSystem);

	addHeading1(mGuiSystem, new Button{"Exit", [this](Button& ref) {
		this->mUpdateOp = sfz::SCREEN_QUIT;
	}}, buttonWidth);
	addStandardPadding(mGuiSystem);

	// Add navbar
	mGuiSystem.addSpacing(restPadding - NAVBAR_HEIGHT * 4.0f);
	mGuiSystem.addSpacing(NAVBAR_PADDING);
	mGuiSystem.addItem(shared_ptr<BaseItem>(new ImageItem(a.CREDITS_LOGO)), vec2{MENU_DIM.x, NAVBAR_HEIGHT * 5.0f});
	mGuiSystem.addSpacing(MENU_BOTTOM_PADDING);
}

// MainMenuScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp MainMenuScreen::update(UpdateState& state)
{
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (cancelRef) return sfz::SCREEN_QUIT;
	mGuiSystem.update(data, state.delta);

	return mUpdateOp;
}

void MainMenuScreen::render(UpdateState& state)
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