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

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Grid width", {"2", "3", "4", "5", "6"}, [this]() {
		int gw = this->cfgData.modelConfig.gridWidth;
		if (2 <= gw && gw <= 6) return gw-2;
		return -1;
	}, [this](int choice) {
		this->cfgData.modelConfig.gridWidth = (choice+2);
	}, stateAlignOffset}});


	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Movement speed", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>(createLinearFloatMultiChoiceSelector("Base speed", &cfgData.modelConfig.tilesPerSecond, 0.5f, 0.25f, 19, stateAlignOffset, " tiles/s", 2)));

	addHeading3(scrollList, shared_ptr<BaseItem>(createLinearFloatMultiChoiceSelector("Speed increase (per object)", &cfgData.modelConfig.speedIncreasePerObject, 0.0f, 0.005f, 21, stateAlignOffset, " tiles/s", 3)));


	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Bonus settings", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Bonus frequency", {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"}, [this]() {
		int val = this->cfgData.modelConfig.bonusFrequency;
		if (val < 0 || 32 < val) return -1;
		return val;
	}, [this](int choice) {
		this->cfgData.modelConfig.bonusFrequency = choice;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Bonus duration", {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"}, [this]() {
		int val = this->cfgData.modelConfig.bonusDuration;
		if (val < 0 || 32 < val) return -1;
		return val;
	}, [this](int choice) {
		this->cfgData.modelConfig.bonusDuration = choice;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Number of bonus objects", {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"}, [this]() {
		int val = this->cfgData.modelConfig.numberOfBonusObjects;
		if (val < 0 || 16 < val) return -1;
		return val;
	}, [this](int choice) {
		this->cfgData.modelConfig.numberOfBonusObjects = choice;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Early bonus duration", {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"}, [this]() {
		int val = this->cfgData.modelConfig.earlyDuration;
		if (val < 0 || 32 < val) return -1;
		return val;
	}, [this](int choice) {
		this->cfgData.modelConfig.earlyDuration = choice;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Shift bonus duration", {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"}, [this]() {
		int val = this->cfgData.modelConfig.shiftBonusDuration;
		if (val < 0 || 32 < val) return -1;
		return val;
	}, [this](int choice) {
		this->cfgData.modelConfig.shiftBonusDuration = choice;
	}, stateAlignOffset}});


	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Scoring", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Object value", {"0", "8", "16", "24", "32", "40", "48", "56", "64", "72", "80", "88", "96", "104", "112", "120", "128"}, [this]() {
		int val = this->cfgData.modelConfig.objectValue;
		if (val < 0 || 128 < val) return -1;
		if ((val % 8) != 0) return -1;
		return val / 8;
	}, [this](int choice) {
		this->cfgData.modelConfig.objectValue = choice * 8;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Object early bonus value", {"0", "8", "16", "24", "32", "40", "48", "56", "64", "72", "80", "88", "96", "104", "112", "120", "128"}, [this]() {
		int val = this->cfgData.modelConfig.objectEarlyBonus;
		if (val < 0 || 128 < val) return -1;
		if ((val % 8) != 0) return -1;
		return val / 8;
	}, [this](int choice) {
		this->cfgData.modelConfig.objectEarlyBonus = choice * 8;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Object shift bonus value", {"0", "8", "16", "24", "32", "40", "48", "56", "64", "72", "80", "88", "96", "104", "112", "120", "128"}, [this]() {
		int val = this->cfgData.modelConfig.objectShiftBonus;
		if (val < 0 || 128 < val) return -1;
		if ((val % 8) != 0) return -1;
		return val / 8;
	}, [this](int choice) {
		this->cfgData.modelConfig.objectShiftBonus = choice * 8;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Bonus object value", {"0", "8", "16", "24", "32", "40", "48", "56", "64", "72", "80", "88", "96", "104", "112", "120", "128"}, [this]() {
		int val = this->cfgData.modelConfig.bonusObjectValue;
		if (val < 0 || 128 < val) return -1;
		if ((val % 8) != 0) return -1;
		return val / 8;
	}, [this](int choice) {
		this->cfgData.modelConfig.bonusObjectValue = choice * 8;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Bonus object shift bonus value", {"0", "8", "16", "24", "32", "40", "48", "56", "64", "72", "80", "88", "96", "104", "112", "120", "128"}, [this]() {
		int val = this->cfgData.modelConfig.bonusObjectShiftBonus;
		if (val < 0 || 128 < val) return -1;
		if ((val % 8) != 0) return -1;
		return val / 8;
	}, [this](int choice) {
		this->cfgData.modelConfig.bonusObjectShiftBonus = choice * 8;
	}, stateAlignOffset}});
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