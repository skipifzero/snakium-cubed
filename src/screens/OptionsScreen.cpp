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
#include "screens/OptionsCustomScreen.hpp"
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
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Game settings", HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{MultiChoiceSelector::createLinearInteger("Input buffer size", &cfgData.inputBufferSize, 1, 1, 5, stateAlignOffset, " frames")});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Custom model preset", {"Standard", "Large", "Giant", "Custom"}, [this]() {
		if (this->cfgData.modelConfig == STANDARD_CONFIG) return 0;
		if (this->cfgData.modelConfig == LARGE_CONFIG) return 1;
		if (this->cfgData.modelConfig == GIANT_CONFIG) return 2;
		return 3;
	}, [this](int choice) {
		switch (choice) {
		case 0:
			this->cfgData.modelConfig = STANDARD_CONFIG;
			break;
		case 1:
			this->cfgData.modelConfig = LARGE_CONFIG;
			break;
		case 2:
			this->cfgData.modelConfig = GIANT_CONFIG;
			break;
		case 3:
			this->cfgData.modelConfig.tilesPerSecond = 2.51f;
			break;
		}
	}, stateAlignOffset}});

	addHeading2(scrollList, shared_ptr<BaseItem>{new Button{"Custom model", [this](Button&) {
		this->applyConfig();
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new OptionsCustomScreen{}}};
	}}}, buttonWidth, gl::HorizontalAlign::LEFT);


	// Debug
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Debug", HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{OnOffSelector::createSimple("Print frametimes", &cfgData.printFrametimes, stateAlignOffset)});

	addHeading3(scrollList, shared_ptr<BaseItem>{OnOffSelector::createSimple("Continuous shader reload", &cfgData.continuousShaderReload, stateAlignOffset)});
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