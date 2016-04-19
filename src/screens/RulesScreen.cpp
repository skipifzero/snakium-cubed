#include "screens/RulesScreen.hpp"

#include <sfz/gl/OpenGL.hpp>

#include "gamelogic/ModelConfig.hpp"
#include "GlobalConfig.hpp"
#include "screens/GameScreen.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

// RulesScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

RulesScreen::RulesScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;

	const float buttonWidth = MENU_DIM.x * 0.4f;
	const float restPadding = calcRestPadding(0.0f, 3.0f, 10.0f, 3.0f);
	const float rulesWidth = MENU_DIM.x * 0.95f;

	addTitle(mGuiSystem, new TextItem{"Rules"});
	addStandardPadding(mGuiSystem);

	addHeading2(mGuiSystem, new TextItem{"Objects (blue)", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- Early bonus if eaten shortly after appearance", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- Shift bonus if eaten shortly after a shift", gl::HorizontalAlign::LEFT}, rulesWidth);
	addStandardPadding(mGuiSystem);

	addHeading2(mGuiSystem, new TextItem{"Bonus objects (red)", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- Appears after a number of objects have been eaten", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- Rewards more points than regular objects", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- Disappears if not eaten shortly after appearance", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- Shift bonus if eaten shortly after a shift", gl::HorizontalAlign::LEFT}, rulesWidth);
	addStandardPadding(mGuiSystem);

	addHeading2(mGuiSystem, new TextItem{"Hints", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- Shift through the cube by pressing Space or A", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- Objects lose the blue glow when you are late", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- The snake glows more strongly when you have shift bonus", gl::HorizontalAlign::LEFT}, rulesWidth);
	addHeading3(mGuiSystem, new TextItem{"- The shift bonus is only active a very short amount of time", gl::HorizontalAlign::LEFT}, rulesWidth);

	addNavbar(mGuiSystem, new Button{"Back", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		       shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}, restPadding, buttonWidth);
}

// RulesScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp RulesScreen::update(UpdateState& state)
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

void RulesScreen::render(UpdateState& state)
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