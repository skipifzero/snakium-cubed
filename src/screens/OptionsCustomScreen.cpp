#include "screens/OptionsCustomScreen.hpp"

#include <algorithm>
#include <memory>

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>
#include <sfz/math/MathHelpers.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "screens/GameScreen.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/OptionsScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

// OptionsCustomScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

OptionsCustomScreen::OptionsCustomScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;
	cfgData = GlobalConfig::INSTANCE().data();

	// Constants
	const float stateAlignOffset = MENU_DIM.x * 0.535f;
	const float buttonWidth = 42.0f;

	// Title, scrollList and navbar
	addTitle(mGuiSystem, new TextItem{"Custom Model"});
	mGuiSystem.addItem(shared_ptr<BaseItem>{new ScrollListContainer{7.5f}}, SPACE_AVAILABLE);
	ScrollListContainer& scrollList = *(ScrollListContainer*)mGuiSystem.items().back().get();
	
	mCancelApplyCon = shared_ptr<BaseItem>{new SideSplitContainer{}};
	addNavbar(mGuiSystem, mCancelApplyCon, 0.0f);
	SideSplitContainer& sideSplit = *(SideSplitContainer*)mGuiSystem.items().back().get();

	mCancelButton = shared_ptr<BaseItem>{new Button{"Cancel", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new OptionsScreen{}}};
	}}};
	sideSplit.setLeft(mCancelButton, MENU_DIM.x * 0.4f);

	mApplyButton = shared_ptr<BaseItem>{new Button{"Apply", [this](Button&) {
		this->applyConfig();
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new OptionsScreen{}}};
	}}};
	sideSplit.setRight(mApplyButton, MENU_DIM.x * 0.4f);

	// ScrollList: CustomModel
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Cube size", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Grid width", &cfgData.modelConfig.gridWidth, 2, 1, 5, stateAlignOffset)});

	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Movement speed", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>(createLinearFloatMultiChoiceSelector("Base speed", &cfgData.modelConfig.tilesPerSecond, 0.5f, 0.25f, 19, stateAlignOffset, " tiles/s", 2)));

	addHeading3(scrollList, shared_ptr<BaseItem>(createLinearFloatMultiChoiceSelector("Speed increase (per object)", &cfgData.modelConfig.speedIncreasePerObject, 0.0f, 0.005f, 21, stateAlignOffset, " tiles/s", 3)));


	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Bonus settings", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Bonus frequency", &cfgData.modelConfig.bonusFrequency, 0, 1, 33, stateAlignOffset)});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Bonus duration", &cfgData.modelConfig.bonusDuration, 0, 1, 33, stateAlignOffset, " tiles")});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Number of bonus objects", &cfgData.modelConfig.numberOfBonusObjects, 0, 1, 17, stateAlignOffset)});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Early bonus duration", &cfgData.modelConfig.earlyDuration, 0, 1, 33, stateAlignOffset, " tiles")});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Shift bonus duration", &cfgData.modelConfig.shiftBonusDuration, 0, 1, 33, stateAlignOffset, " tiles")});


	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Scoring", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Object value", &cfgData.modelConfig.objectValue, 0, 8, 17, stateAlignOffset, " points")});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Object early bonus value", &cfgData.modelConfig.objectEarlyBonus, 0, 8, 17, stateAlignOffset, " points")});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Object shift bonus value", &cfgData.modelConfig.objectShiftBonus, 0, 8, 17, stateAlignOffset, " points")});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Bonus object value", &cfgData.modelConfig.bonusObjectValue, 0, 8, 17, stateAlignOffset, " points")});

	addHeading3(scrollList, shared_ptr<BaseItem>{createLinearIntegerMultiChoiceSelector("Bonus object shift bonus value", &cfgData.modelConfig.bonusObjectShiftBonus, 0, 8, 17, stateAlignOffset, " points")});
}

// OptionsCustomScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp OptionsCustomScreen::update(UpdateState& state)
{
	if (mWindowPtr == nullptr) mWindowPtr = &state.window;

	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (cancelRef) {
		if (cfgData == GlobalConfig::INSTANCE().data()) {
			this->applyConfig();
			return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			                shared_ptr<BaseScreen>{new MainMenuScreen{}}};
		}

		// Awful, awful hack to select apply button if back is pressed when changes have been made.
		vec2 oldPointerPos = data.pointerPos;
		data.pointerPos = mGuiSystem.bounds().position() + mCancelApplyCon->offset + mApplyButton->offset;
		data.pointerMotion += (data.pointerPos - oldPointerPos);
	}
	mGuiSystem.update(data, state.delta);

	return mUpdateOp;
}

void OptionsCustomScreen::render(UpdateState& state)
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

// OptionsCustomScreen: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void OptionsCustomScreen::applyConfig() noexcept
{
	auto& globalCfg = GlobalConfig::INSTANCE();
	
	// Add the new settings to Global Config
	globalCfg.data(cfgData);

	// Enable new settings
	mWindowPtr->setVSync(static_cast<sdl::VSync>(cfgData.gc.vsync));
	mWindowPtr->setFullscreen(static_cast<sdl::Fullscreen>(cfgData.fullscreenMode), cfgData.displayIndex);
	
	// Write to file
	globalCfg.save();
}

} // namespace s3