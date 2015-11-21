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
#include "screens/ScreenMenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct Resolutions {
	vector<string> names;
	vector<SDL_DisplayMode> modes;
};

static Resolutions getAvailableResolutions(ConfigData& data) noexcept
{
	// Make sure current display index is valid
	const int numDisplays = SDL_GetNumVideoDisplays();
	if (numDisplays < 0) {
		std::cerr << "SDL_GetNumVideoDisplays() failed: " << SDL_GetError() << std::endl;
	}
	if (data.displayIndex >= numDisplays) {
		std::cerr << "Display index " << data.displayIndex << " is invalid, max is "
		          << numDisplays << ". Resetting to 0." << std::endl;
		data.displayIndex = 0;
	}

	// Get all available display modes for the selected display index
	vector<SDL_DisplayMode> modes;
	SDL_DisplayMode mode;
	const int numDisplayModes = SDL_GetNumDisplayModes(data.displayIndex);
	if (numDisplayModes < 0) {
		std::cerr << "SDL_GetNumDisplayModes failed: " << SDL_GetError() << std::endl;
	}
	for (int i = 0; i < numDisplayModes; ++i) {
		mode = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
		if (SDL_GetDisplayMode(data.displayIndex, i, &mode) != 0) {
			std::cerr << "SDL_GetDisplayMode failed: " << SDL_GetError() << std::endl;
		}
		modes.push_back(mode);
		//std::cout << "DisplayMode: " << mode.w << "x" << mode.h << ", " << mode.refresh_rate << "Hz, PixelFormat: " << mode.format << std::endl;
	}

	// Sort the display modes
	std::sort(modes.begin(), modes.end(), [](const SDL_DisplayMode& lhs, const SDL_DisplayMode& rhs) {
		if (lhs.w == rhs.w && lhs.h == rhs.h) {
			return lhs.refresh_rate < rhs.refresh_rate;
		}
		float lhsSquaredLength = sfz::squaredLength(vec2{(float)lhs.w, (float)lhs.h});
		float rhsSquaredLength = sfz::squaredLength(vec2{(float)rhs.w, (float)rhs.h});
		return lhsSquaredLength < rhsSquaredLength;
	});

	// Create display mode strings
	vector<string> names;
	for (auto& m : modes) {
		names.emplace_back(std::to_string(m.w) + "x" + std::to_string(m.h) + " (" + std::to_string(m.refresh_rate) + "Hz)");
	}

	Resolutions temp;
	temp.names = std::move(names);
	temp.modes = std::move(modes);
	return temp;
};


// OptionsScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

OptionsScreen::OptionsScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE}}
{
	cfgData = GlobalConfig::INSTANCE().data();

	using namespace gui;

	const vec2 menuDim = vec2{screens::MIN_DRAWABLE.x-0.1f, screens::MIN_DRAWABLE.y-0.1f};
	const float spacing = 3.5f;
	const float itemSpacing = 1.8f;
	const float bottomSpacing = 2.0f;
	const float titleHeight = 20.0f;
	const float buttonHeight = 8.0f;
	const float stateAlignOffset = menuDim.x * 0.535f;
	const vec2 headingDim{menuDim.x * 0.95f, 7.5f};
	const vec2 infoTextDim{menuDim.x * 0.95f, 3.0f};
	const vec2 itemDim{menuDim.x * 0.95f, 4.6725f};
	float scrollListHeight = menuDim.y - titleHeight - buttonHeight - 2.0f*spacing - bottomSpacing;

	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{"Options"}}, vec2{menuDim.x, titleHeight});
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new ScrollListContainer{7.5f}},
	                   vec2{menuDim.x, scrollListHeight});

	// Graphics
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ScrollListContainer& scrollList = *(ScrollListContainer*)mGuiSystem.items().back().get();
	scrollList.addItem(shared_ptr<BaseItem>{new TextItem{"Graphics", HorizontalAlign::LEFT}}, headingDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Fullscreen", {"Off", "Windowed", "Exclusive"}, [this]() {
		return this->cfgData.fullscreenMode;
	}, [this](int choice) {
		this->cfgData.fullscreenMode = choice;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	auto temp = getAvailableResolutions(cfgData);
	mDisplayModes = std::move(temp.modes);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Resolution", std::move(temp.names), [this]() {
		for (int i = 0; i < (int)this->mDisplayModes.size(); ++i) {
			auto m = this->mDisplayModes[i];
			auto& d = this->cfgData;
			if (m.w == d.resolutionX && m.h == d.resolutionY && m.refresh_rate == d.refreshRate) {
				return i;
			}
		}
		return -1;
	}, [this](int choice) {
		this->cfgData.resolutionX = this->mDisplayModes[choice].w;
		this->cfgData.resolutionY = this->mDisplayModes[choice].h;
		this->cfgData.refreshRate = this->mDisplayModes[choice].refresh_rate;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"VSync", {"Off", "On", "Swap Control Tear"}, [this]() {
		return this->cfgData.vsync;
	}, [this](int choice) {
		this->cfgData.vsync = choice;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new TextItem{"Internal resolution is a factor of the current output resolution", HorizontalAlign::LEFT}}, infoTextDim);
	scrollList.addSpacing(itemSpacing);
	mInternalResMultiChoicePtr = shared_ptr<BaseItem>{new MultiChoiceSelector{"Internal Resolution", {}, [this]() {
		float val = this->cfgData.internalResScaling;
		const float eps = 0.01f;
		int i = 0;
		for (float factor = 0.05f; factor <= 4.0f; factor += 0.05f) {
			if (sfz::approxEqual(val, factor, eps)) return i;
			++i;
		}
		return -1;
	}, [this](int choice) {
		this->cfgData.internalResScaling = 0.05f + ((float)choice)*0.05f;
		this->mDrawableDim = vec2{-1.0f};
	}, stateAlignOffset}};
	scrollList.addItem(mInternalResMultiChoicePtr, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new TextItem{"The following resolutions are factors of the internal resolution", HorizontalAlign::LEFT}}, infoTextDim);
	scrollList.addSpacing(itemSpacing);
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
	scrollList.addItem(mBlurResMultiChoicePtr, itemDim);

	scrollList.addSpacing(itemSpacing);
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
	scrollList.addItem(mSpotlightResMultiChoicePtr, itemDim);

	scrollList.addSpacing(itemSpacing);
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
	scrollList.addItem(mLightShaftResMultiChoicePtr, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Scaling Algorithm", { "Nearest", "Bilinear", "2x2 Nearest", "2x2 Bilinear", "4x4 Nearest", "4x4 Bilinear", "Bicubic Bspline", "Lanczos-2", "Lanczos-3"}, [this]() {
		return this->cfgData.scalingAlgorithm;
	}, [this](int choice) {
		this->cfgData.scalingAlgorithm = choice;
	}, stateAlignOffset}}, itemDim);


	// GameSettings
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	scrollList.addSpacing(spacing);
	scrollList.addItem(shared_ptr<BaseItem>{new TextItem{"Game Settings", HorizontalAlign::LEFT}}, headingDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Input Buffer Size", {"1", "2", "3", "4", "5"}, [this]() {
		int bs = this->cfgData.inputBufferSize;
		if (1 <= bs && bs <= 5) return bs-1;
		return -1;
	}, [this](int choice) {
		this->cfgData.inputBufferSize = (choice+1);
	}, stateAlignOffset}}, itemDim);

	// CustomModel
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	scrollList.addSpacing(spacing);
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
	}, stateAlignOffset}}, itemDim);

	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new SideSplitContainer{}}, vec2{menuDim.x, buttonHeight});
	SideSplitContainer& sideSplit = *(SideSplitContainer*)mGuiSystem.items().back().get();
	
	sideSplit.setLeft(shared_ptr<BaseItem>{new Button{"Cancel", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}}, menuDim.x * 0.4f);

	sideSplit.setRight(shared_ptr<BaseItem>{new Button{"Apply", [this](Button&) {
		this->applyConfig();
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}}, menuDim.x * 0.4f);
}

// OptionsScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp OptionsScreen::update(UpdateState& state)
{
	if (mWindowPtr == nullptr) mWindowPtr = &state.window;

	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, screens::MIN_DRAWABLE);

	if (mDrawableDim != drawableDim) {
		mDrawableDim = drawableDim;
		updateResolutionStrings();
	}

	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (cancelRef) {
		this->applyConfig();
		return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}
	mGuiSystem.update(data, state.delta);

	return mUpdateOp;
}

void OptionsScreen::render(UpdateState& state)
{
	// Clearing screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Sizes
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, screens::MIN_DRAWABLE);

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
	
	bool needToUpdateFullscreen = (globalCfg.fullscreenMode == 2);

	// Add the new settings to Global Config
	globalCfg.data(cfgData);

	// Enable new settings

	// Resolution
	SDL_DisplayMode cfgDataMode;
	cfgDataMode.w = cfgData.resolutionX;
	cfgDataMode.h = cfgData.resolutionY;
	cfgDataMode.format = 0;
	cfgDataMode.refresh_rate = cfgData.refreshRate;
	cfgDataMode.driverdata = 0;
	SDL_DisplayMode closest;
	if (SDL_GetClosestDisplayMode(cfgData.displayIndex, &cfgDataMode, &closest) == NULL) {
		std::cerr << "SDL_GetClosestDisplayMode() failed: " << SDL_GetError() << std::endl;
	}
	if (SDL_SetWindowDisplayMode(mWindowPtr->mPtr, &closest) < 0) {
		std::cerr << "SDL_SetWindowDisplayMode() failed: " << SDL_GetError() << std::endl;
	}

	// Fullscreen
	int fullscreenFlags = 0;
	if (cfgData.fullscreenMode == 0) fullscreenFlags = 0;
	else if (cfgData.fullscreenMode == 1) fullscreenFlags = SDL_WINDOW_FULLSCREEN_DESKTOP;
	else if (cfgData.fullscreenMode == 2) fullscreenFlags = SDL_WINDOW_FULLSCREEN;
	if (SDL_SetWindowFullscreen(mWindowPtr->mPtr, fullscreenFlags) < 0) {
		std::cerr << "SDL_SetWindowFullscreen() failed: " << SDL_GetError() << std::endl;
	}
	if (needToUpdateFullscreen && fullscreenFlags == SDL_WINDOW_FULLSCREEN) {
		if (SDL_SetWindowFullscreen(mWindowPtr->mPtr, 0) < 0) {
			std::cerr << "SDL_SetWindowFullscreen() failed: " << SDL_GetError() << std::endl;
		}
		if (SDL_SetWindowFullscreen(mWindowPtr->mPtr, SDL_WINDOW_FULLSCREEN) < 0) {
			std::cerr << "SDL_SetWindowFullscreen() failed: " << SDL_GetError() << std::endl;
		}
	}
	
	// VSync
	int vsyncInterval = 1;
	if (cfgData.vsync == 0) vsyncInterval = 0;
	else if (cfgData.vsync == 1) vsyncInterval = 1;
	else if (cfgData.vsync == 2) vsyncInterval = -1;
	if (SDL_GL_SetSwapInterval(vsyncInterval) < 0) {
		std::cerr << "SDL_GL_SetSwapInterval() failed: " << SDL_GetError() << std::endl;
	}
	
	// Write to file
	globalCfg.save();
}

void OptionsScreen::updateResolutionFactors() noexcept
{
	char buffer[128];
	mInternalResFactorStrs.clear();
	mSecondaryResFactorStrs.clear();
	vec2i internalRes{int(mOutputRes.x * cfgData.internalResScaling),
	                  int(mOutputRes.y * cfgData.internalResScaling)};
	for (float factor = 0.05f; factor <= 4.0f; factor += 0.05) {
		std::snprintf(buffer, 128, "%ix%i (%.2f)", int(mOutputRes.x * factor), 
		              int(mOutputRes.y * factor), factor);
		mInternalResFactorStrs.push_back(buffer);
		std::snprintf(buffer, 128, "%ix%i (%.2f)", int(internalRes.x * factor), 
		              int(internalRes.y * factor), factor);
		mSecondaryResFactorStrs.push_back(buffer);
	}
}

void OptionsScreen::updateResolutionStrings() noexcept
{
	using namespace gui;
	mOutputRes = vec2i{(int)mDrawableDim.x, (int)mDrawableDim.y};
	this->updateResolutionFactors();

	MultiChoiceSelector& internalResMultiChoice = *static_cast<MultiChoiceSelector*>(mInternalResMultiChoicePtr.get());
	MultiChoiceSelector& blurResMultiChoice = *static_cast<MultiChoiceSelector*>(mBlurResMultiChoicePtr.get());
	MultiChoiceSelector& spotlightResMultiChoice = *static_cast<MultiChoiceSelector*>(mSpotlightResMultiChoicePtr.get());
	MultiChoiceSelector& lightShaftResMultiChoice = *static_cast<MultiChoiceSelector*>(mLightShaftResMultiChoicePtr.get());

	internalResMultiChoice.choiceNames = mInternalResFactorStrs;
	blurResMultiChoice.choiceNames = vector<string>{mSecondaryResFactorStrs.begin(), mSecondaryResFactorStrs.begin()+20};
	spotlightResMultiChoice.choiceNames = mSecondaryResFactorStrs;
	lightShaftResMultiChoice.choiceNames = mSecondaryResFactorStrs;
}

} // namespace s3