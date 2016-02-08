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
#include "screens/MenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static vector<int> availableVerticalResolutions() noexcept
{
	auto& cfgData = GlobalConfig::INSTANCE().data();
	auto tmp = sdl::getAvailableResolutions();
	
	vector<int> resolutions;
	for (auto& res : tmp) {
		resolutions.push_back(res.y);
	}
	
	resolutions.push_back(cfgData.internalResolutionY);
	resolutions.push_back(240);
	resolutions.push_back(360);
	resolutions.push_back(480);
	resolutions.push_back(576);
	resolutions.push_back(600);
	resolutions.push_back(720);
	resolutions.push_back(800);
	resolutions.push_back(900);
	resolutions.push_back(1080);
	resolutions.push_back(1200);
	resolutions.push_back(1440);
	resolutions.push_back(1600);
	resolutions.push_back(2160);
	resolutions.push_back(2400);
	resolutions.push_back(2880);
	resolutions.push_back(3240);
	resolutions.push_back(4320);

	// Sort
	std::sort(resolutions.begin(), resolutions.end(), [](int lhs, int rhs) {
		return lhs < rhs;
	});

	// Remove duplicates
	for (int i = 1; i < resolutions.size(); ++i) {
		if (resolutions[i] == resolutions[i-1]) {
			resolutions.erase(resolutions.begin() + i);
			i -= 1;
		}
	}
	
	return std::move(resolutions);
}

// OptionsScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

OptionsScreen::OptionsScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;
	cfgData = GlobalConfig::INSTANCE().data();
	mYResolutions = availableVerticalResolutions();

	// Constants
	const float stateAlignOffset = MENU_DIM.x * 0.535f;

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

	addHeading1(scrollList, shared_ptr<BaseItem>{new TextItem{"Graphics", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Fullscreen", {"Off", "Windowed", "Exclusive"}, [this]() {
		return this->cfgData.fullscreenMode;
	}, [this](int choice) {
		this->cfgData.fullscreenMode = choice;
	}, stateAlignOffset}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"VSync", {"Off", "On", "Swap Control Tear"}, [this]() {
		return this->cfgData.vsync;
	}, [this](int choice) {
		this->cfgData.vsync = choice;
	}, stateAlignOffset}});

	mInternalResMultiChoicePtr = shared_ptr<BaseItem>{new MultiChoiceSelector{"Internal Y-Resolution", {}, [this]() {
		int val = this->cfgData.internalResolutionY;
		auto itr = std::find(this->mYResolutions.begin(), this->mYResolutions.end(), val);
		if (itr == this->mYResolutions.end()) return -1;
		return static_cast<int>(itr - this->mYResolutions.begin());
	}, [this](int choice) {
		this->cfgData.internalResolutionY = this->mYResolutions[choice];
		this->mDrawableDim = vec2{-1.0f};
	}, stateAlignOffset}};

	addHeading3(scrollList, mInternalResMultiChoicePtr);

	addHeading3(scrollList, shared_ptr<BaseItem>{new TextItem{"The following resolutions are factors of the internal resolution", HorizontalAlign::LEFT}});

	mBlurResMultiChoicePtr = shared_ptr<BaseItem>{new MultiChoiceSelector{"Emissive Blur Resolution", {}, [this]() {
		float val = this->cfgData.blurResScaling;
		const float eps = 0.01f;
		int i = 0;
		for (float factor = 0.05f; factor <= 4.0f; factor += 0.05f) {
			if (sfz::approxEqual(val, factor, eps)) return i;
			++i;
		}
		return -1;
	}, [this](int choice) {
		this->cfgData.blurResScaling = 0.05f + ((float)choice)*0.05f;
	}, stateAlignOffset}};
	addHeading3(scrollList, mBlurResMultiChoicePtr);
	
	mSpotlightResMultiChoicePtr = shared_ptr<BaseItem>{new MultiChoiceSelector{"Spotlight Resolution", {}, [this]() {
		float val = this->cfgData.spotlightResScaling;
		const float eps = 0.01f;
		int i = 0;
		for (float factor = 0.05f; factor <= 4.0f; factor += 0.05f) {
			if (sfz::approxEqual(val, factor, eps)) return i;
			++i;
		}
		return -1;
	}, [this](int choice) {
		this->cfgData.spotlightResScaling = 0.05f + ((float)choice)*0.05f;
	}, stateAlignOffset}};
	addHeading3(scrollList, mSpotlightResMultiChoicePtr);
	
	mLightShaftResMultiChoicePtr = shared_ptr<BaseItem>{new MultiChoiceSelector{"Light Shaft Resolution", {}, [this]() {
		float val = this->cfgData.lightShaftsResScaling;
		const float eps = 0.01f;
		int i = 0;
		for (float factor = 0.05f; factor <= 4.0f; factor += 0.05f) {
			if (sfz::approxEqual(val, factor, eps)) return i;
			++i;
		}
		return -1;
	}, [this](int choice) {
		this->cfgData.lightShaftsResScaling = 0.05f + ((float)choice)*0.05f;
	}, stateAlignOffset}};
	addHeading3(scrollList, mLightShaftResMultiChoicePtr);
	
	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Scaling Algorithm", {"Nearest", "Bilinear", "2x2 Nearest", "2x2 Bilinear", "4x4 Nearest", "4x4 Bilinear", "Bicubic Bspline", "Lanczos-2", "Lanczos-3"}, [this]() {
		return this->cfgData.scalingAlgorithm;
	}, [this](int choice) {
		this->cfgData.scalingAlgorithm = choice;
	}, stateAlignOffset}});

	// ScrollList: GameSettings
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	addHeading1(scrollList, shared_ptr<BaseItem>{new TextItem{"Game Settings", HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Input Buffer Size", {"1", "2", "3", "4", "5"}, [this]() {
		int bs = this->cfgData.inputBufferSize;
		if (1 <= bs && bs <= 5) return bs-1;
		return -1;
	}, [this](int choice) {
		this->cfgData.inputBufferSize = (choice+1);
	}, stateAlignOffset}});

	// Debug
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	addHeading1(scrollList, shared_ptr<BaseItem>{new TextItem{"Debug", HorizontalAlign::LEFT}});

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

	if (mDrawableDim != drawableDim) {
		mDrawableDim = drawableDim;
		updateResolutionStrings();
	}

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

void OptionsScreen::onResize(vec2 windowDimensions, vec2 drawableDimensions)
{
	mDrawableDim = drawableDimensions;
	updateResolutionStrings();
}

// OptionsScreen: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void OptionsScreen::applyConfig() noexcept
{
	auto& globalCfg = GlobalConfig::INSTANCE();
	
	// Add the new settings to Global Config
	globalCfg.data(cfgData);

	// Enable new settings
	mWindowPtr->setVSync(static_cast<sdl::VSync>(cfgData.vsync));
	mWindowPtr->setFullscreen(static_cast<sdl::Fullscreen>(cfgData.fullscreenMode), cfgData.displayIndex);
	
	// Write to file
	globalCfg.save();
}

void OptionsScreen::updateResolutionFactors() noexcept
{
	char buffer[128];
	const float aspect = mDrawableDim.x / mDrawableDim.y;

	mInternalResStrs.clear();
	for (int i = 0; i < mYResolutions.size(); ++i) {
		std::snprintf(buffer, sizeof(buffer), "%ip (%ix%i)", mYResolutions[i],
		              (int)std::round(cfgData.internalResolutionY * aspect), mYResolutions[i]);
		mInternalResStrs.push_back(buffer);
	}

	mSecondaryResFactorStrs.clear();
	vec2i internalRes{(int)std::round(cfgData.internalResolutionY * aspect), cfgData.internalResolutionY};
	for (float factor = 0.05f; factor <= 4.0f; factor += 0.05) {
		std::snprintf(buffer, 128, "%ix%i (%.2f)", int(std::round(internalRes.x * factor)), 
		              int(std::round(internalRes.y * factor)), factor);
		mSecondaryResFactorStrs.push_back(buffer);
	}
}

void OptionsScreen::updateResolutionStrings() noexcept
{
	using namespace gui;
	this->updateResolutionFactors();

	MultiChoiceSelector& internalResMultiChoice = *static_cast<MultiChoiceSelector*>(mInternalResMultiChoicePtr.get());
	MultiChoiceSelector& blurResMultiChoice = *static_cast<MultiChoiceSelector*>(mBlurResMultiChoicePtr.get());
	MultiChoiceSelector& spotlightResMultiChoice = *static_cast<MultiChoiceSelector*>(mSpotlightResMultiChoicePtr.get());
	MultiChoiceSelector& lightShaftResMultiChoice = *static_cast<MultiChoiceSelector*>(mLightShaftResMultiChoicePtr.get());

	internalResMultiChoice.choiceNames = mInternalResStrs;
	blurResMultiChoice.choiceNames = vector<string>{mSecondaryResFactorStrs.begin(), mSecondaryResFactorStrs.begin()+20};
	spotlightResMultiChoice.choiceNames = mSecondaryResFactorStrs;
	lightShaftResMultiChoice.choiceNames = mSecondaryResFactorStrs;
}

} // namespace s3