#include "screens/OptionsScreen.hpp"

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
#include "screens/OptionsGraphicsScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

// OptionsScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

OptionsScreen::OptionsScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;
	cfgData = GlobalConfig::INSTANCE().data();

	// Constants
	const float stateAlignOffset = MENU_DIM.x * 0.535f;
	const float buttonWidth = 42.0f;

	// Title, scrollList and navbar
	addTitle(mGuiSystem, new TextItem{"Options"});
	mGuiSystem.addItem(shared_ptr<BaseItem>{new ScrollListContainer{7.5f}}, SPACE_AVAILABLE);
	ScrollListContainer& scrollList = *(ScrollListContainer*)mGuiSystem.items().back().get();
	
	mCancelApplyCon = shared_ptr<BaseItem>{new SideSplitContainer{}};
	addNavbar(mGuiSystem, mCancelApplyCon, 0.0f);
	SideSplitContainer& sideSplit = *(SideSplitContainer*)mGuiSystem.items().back().get();

	mCancelButton = shared_ptr<BaseItem>{new Button{"Cancel", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}};
	sideSplit.setLeft(mCancelButton, MENU_DIM.x * 0.4f);

	mApplyButton = shared_ptr<BaseItem>{new Button{"Apply", [this](Button&) {
		this->applyConfig();
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}};
	sideSplit.setRight(mApplyButton, MENU_DIM.x * 0.4f);

	// ScrollList: Graphics
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Graphics", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Fullscreen", {"Off", "Windowed", "Exclusive"}, [this]() {
		return this->cfgData.fullscreenMode;
	}, [this](int choice) {
		this->cfgData.fullscreenMode = choice;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Preset", {"Toaster", "Laptop (Intel GPU)", "Laptop (Nvidia GPU)", "Gaming computer", "Future supercomputer", "I-know-what-I'm-doing"}, [this]() {
		if (this->cfgData.gc == TOASTER_GRAPHICS_CONFIG) return 0;
		if (this->cfgData.gc == LAPTOP_W_INTEL_GRAPHICS_CONFIG) return 1;
		if (this->cfgData.gc == LAPTOP_W_NVIDIA_GRAPHICS_CONFIG) return 2;
		if (this->cfgData.gc == GAMING_COMPUTER_GRAPHICS_CONFIG) return 3;
		if (this->cfgData.gc == FUTURE_SUPERCOMPUTER_GRAPHICS_CONFIG) return 4;
		return 5;
	}, [this](int choice) {
		switch (choice) {
		case 0:
			this->cfgData.gc = TOASTER_GRAPHICS_CONFIG;
			break;
		case 1:
			this->cfgData.gc = LAPTOP_W_INTEL_GRAPHICS_CONFIG;
			break;
		case 2:
			this->cfgData.gc = LAPTOP_W_NVIDIA_GRAPHICS_CONFIG;
			break;
		case 3:
			this->cfgData.gc = GAMING_COMPUTER_GRAPHICS_CONFIG;
			break;
		case 4:
			this->cfgData.gc = FUTURE_SUPERCOMPUTER_GRAPHICS_CONFIG;
			break;
		case 5:
			this->cfgData.gc.nativeInternalRes = true;
			break;
		}
	}, stateAlignOffset}});

	addHeading2(scrollList, shared_ptr<BaseItem>{new Button{"Advanced settings", [this](Button&) {
		this->applyConfig();
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new OptionsGraphicsScreen{}}};
	}}}, buttonWidth, gl::HorizontalAlign::LEFT);


	// ScrollList: GameSettings
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Game Settings", HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Input Buffer Size", {"1", "2", "3", "4", "5"}, [this]() {
		int bs = this->cfgData.inputBufferSize;
		if (1 <= bs && bs <= 5) return bs-1;
		return -1;
	}, [this](int choice) {
		this->cfgData.inputBufferSize = (choice+1);
	}, stateAlignOffset}});

	// Debug
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Debug", HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new OnOffSelector{"Print Frametimes", [this]() {
		return this->cfgData.printFrametimes;
	}, [this]() {
		this->cfgData.printFrametimes = !this->cfgData.printFrametimes;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new OnOffSelector{"Continuous Shader Reload", [this]() {
		return this->cfgData.continuousShaderReload;
	}, [this]() {
		this->cfgData.continuousShaderReload = !this->cfgData.continuousShaderReload;
	}, stateAlignOffset}});


	// CustomModel
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	/*scrollList.addSpacing(spacing);
	scrollList.addItem(shared_ptr<BaseItem>{new TextItem{"Custom Model", HorizontalAlign::LEFT}}, headingDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Grid Width", {"2", "3", "4", "5", "6"}, [this]() {
		int gw = this->cfgData.modelConfig.gridWidth;
		if (2 <= gw && gw <= 6) return gw-2;
		return -1;
	}, [this](int choice) {
		this->cfgData.modelConfig.gridWidth = (choice+2);
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Tiles per second", {"0.50", "0.75", "1.00", "1.25", "1.50", "1.75", "2.00", "2.25", "2.50", "2.75", "3.00", "3.25", "3.50", "3.75", "4.00", "4.25", "4.50", "4.75", "5.00"}, [this]() {
		float val = this->cfgData.modelConfig.tilesPerSecond;
		const float eps = 0.01f;
		if (sfz::approxEqual(val, 0.5f, eps)) return 0;
		if (sfz::approxEqual(val, 0.75f, eps)) return 1;
		if (sfz::approxEqual(val, 1.0f, eps)) return 2;
		if (sfz::approxEqual(val, 1.25f, eps)) return 3;
		if (sfz::approxEqual(val, 1.5f, eps)) return 4;
		if (sfz::approxEqual(val, 1.75f, eps)) return 5;
		if (sfz::approxEqual(val, 2.0f, eps)) return 6;
		if (sfz::approxEqual(val, 2.25f, eps)) return 7;
		if (sfz::approxEqual(val, 2.5f, eps)) return 8;
		if (sfz::approxEqual(val, 2.75f, eps)) return 9;
		if (sfz::approxEqual(val, 3.0f, eps)) return 10;
		if (sfz::approxEqual(val, 3.25f, eps)) return 11;
		if (sfz::approxEqual(val, 3.5f, eps)) return 12;
		if (sfz::approxEqual(val, 3.75f, eps)) return 13;
		if (sfz::approxEqual(val, 4.0f, eps)) return 14;
		if (sfz::approxEqual(val, 4.25f, eps)) return 15;
		if (sfz::approxEqual(val, 4.5f, eps)) return 16;
		if (sfz::approxEqual(val, 4.75f, eps)) return 17;
		if (sfz::approxEqual(val, 5.0f, eps)) return 18;
		return -1;
	}, [this](int choice) {
		this->cfgData.modelConfig.tilesPerSecond = 0.5f + ((float)choice)*0.25f;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Speed increase", {"0.000", "0.005", "0.010", "0.015", "0.020", "0.025", "0.030", "0.035", "0.040", "0.045", "0.050", "0.055", "0.060", "0.065", "0.070", "0.075", "0.080", "0.085", "0.090", "0.095", "0.100"}, [this]() {
		float val = this->cfgData.modelConfig.speedIncreasePerObject;
		const float eps = 0.0001f;
		if (sfz::approxEqual(val, 0.000f, eps)) return 0;
		if (sfz::approxEqual(val, 0.005f, eps)) return 1;
		if (sfz::approxEqual(val, 0.010f, eps)) return 2;
		if (sfz::approxEqual(val, 0.015f, eps)) return 3;
		if (sfz::approxEqual(val, 0.020f, eps)) return 4;
		if (sfz::approxEqual(val, 0.025f, eps)) return 5;
		if (sfz::approxEqual(val, 0.030f, eps)) return 6;
		if (sfz::approxEqual(val, 0.035f, eps)) return 7;
		if (sfz::approxEqual(val, 0.040f, eps)) return 8;
		if (sfz::approxEqual(val, 0.045f, eps)) return 9;
		if (sfz::approxEqual(val, 0.050f, eps)) return 10;
		if (sfz::approxEqual(val, 0.055f, eps)) return 11;
		if (sfz::approxEqual(val, 0.060f, eps)) return 12;
		if (sfz::approxEqual(val, 0.065f, eps)) return 13;
		if (sfz::approxEqual(val, 0.070f, eps)) return 14;
		if (sfz::approxEqual(val, 0.075f, eps)) return 15;
		if (sfz::approxEqual(val, 0.080f, eps)) return 16;
		if (sfz::approxEqual(val, 0.085f, eps)) return 17;
		if (sfz::approxEqual(val, 0.090f, eps)) return 18;
		if (sfz::approxEqual(val, 0.095f, eps)) return 19;
		if (sfz::approxEqual(val, 0.100f, eps)) return 20;
		return -1;
	}, [this](int choice) {
		this->cfgData.modelConfig.speedIncreasePerObject = ((float)choice)*0.005f;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Object value", {"0", "8", "16", "24", "32", "40", "48", "56", "64", "72", "80", "88", "96", "104", "112", "120", "128"}, [this]() {
		int val = this->cfgData.modelConfig.objectValue;
		if (val < 0 || 128 < val) return -1;
		if ((val % 8) != 0) return -1;
		return val / 8;
	}, [this](int choice) {
		this->cfgData.modelConfig.objectValue = choice * 8;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new OnOffSelector{"Has Bonus", [this]() {
		return this->cfgData.modelConfig.hasBonus;
	}, [this]() {
		this->cfgData.modelConfig.hasBonus = !this->cfgData.modelConfig.hasBonus;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Bonus Frequency", {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"}, [this]() {
		int val = this->cfgData.modelConfig.bonusFrequency;
		if (val < 0 || 32 < val) return -1;
		return val;
	}, [this](int choice) {
		this->cfgData.modelConfig.bonusFrequency = choice;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Bonus Duration", {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"}, [this]() {
		int val = this->cfgData.modelConfig.bonusDuration;
		if (val < 0 || 32 < val) return -1;
		return val;
	}, [this](int choice) {
		this->cfgData.modelConfig.bonusDuration = choice;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Bonus object value", {"0", "8", "16", "24", "32", "40", "48", "56", "64", "72", "80", "88", "96", "104", "112", "120", "128"}, [this]() {
		int val = this->cfgData.modelConfig.bonusObjectValue;
		if (val < 0 || 128 < val) return -1;
		if ((val % 8) != 0) return -1;
		return val / 8;
	}, [this](int choice) {
		this->cfgData.modelConfig.bonusObjectValue = choice * 8;
	}, stateAlignOffset}}, itemDim);*/

	/*mGuiSystem.addSpacing(spacing);
	mCancelApplyCon = shared_ptr<BaseItem>{new SideSplitContainer{}};
	mGuiSystem.addItem(mCancelApplyCon, vec2{MENU_DIM.x, NAVBAR_HEIGHT});
	SideSplitContainer& sideSplit = *(SideSplitContainer*)mGuiSystem.items().back().get();

	mCancelButton = shared_ptr<BaseItem>{new Button{"Cancel", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}};
	sideSplit.setLeft(mCancelButton, MENU_DIM.x * 0.4f);

	mApplyButton = shared_ptr<BaseItem>{new Button{"Apply", [this](Button&) {
		this->applyConfig();
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}};
	sideSplit.setRight(mApplyButton, MENU_DIM.x * 0.4f);*/
}

// OptionsScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp OptionsScreen::update(UpdateState& state)
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

void OptionsScreen::render(UpdateState& state)
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

// OptionsScreen: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void OptionsScreen::applyConfig() noexcept
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