#include "screens/ModeSelectScreen.hpp"

#include <sfz/gl/OpenGL.hpp>

#include "gamelogic/ModelConfig.hpp"
#include "GlobalConfig.hpp"
#include "screens/GameScreen.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

// ModeSelectScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ModeSelectScreen::ModeSelectScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;

	const float buttonWidth = MENU_DIM.x * 0.4f;
	const float numButtons = 4.0f;

	addTitle(mGuiSystem, new TextItem{"Select Mode"});

	addHeading1(mGuiSystem, new Button{"Standard", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		       shared_ptr<BaseScreen>{new GameScreen{s3::STANDARD_CONFIG}}};
	}}, buttonWidth);
	addStandardPadding(mGuiSystem);

	addHeading1(mGuiSystem, new Button{"Large", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		       shared_ptr<BaseScreen>{new GameScreen{s3::LARGE_CONFIG}}};
	}}, buttonWidth);
	addStandardPadding(mGuiSystem);

	addHeading1(mGuiSystem, new Button{"Giant", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		       shared_ptr<BaseScreen>{new GameScreen{s3::GIANT_CONFIG}}};
	}}, buttonWidth);
	addStandardPadding(mGuiSystem);

	addHeading1(mGuiSystem, new Button{"Custom", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		       shared_ptr<BaseScreen>{new GameScreen{GlobalConfig::INSTANCE().modelConfig}}};
	}}, buttonWidth);
	addStandardPadding(mGuiSystem);

	addNavbar(mGuiSystem, new Button{"Back", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		       shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}, numButtons, 0.0f, 0.0f, numButtons, buttonWidth);
}

// ModeSelectScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp ModeSelectScreen::update(UpdateState& state)
{
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (cancelRef) {
		return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                std::shared_ptr<sfz::BaseScreen>{new MainMenuScreen{}}};
	}
	mGuiSystem.update(data, state.delta);

	return mUpdateOp;
}

void ModeSelectScreen::render(UpdateState& state)
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