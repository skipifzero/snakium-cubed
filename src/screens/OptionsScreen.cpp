#include "screens/OptionsScreen.hpp"

#include <algorithm>
#include <memory>

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/GL.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "rendering/GUIRendering.hpp"
#include "screens/GameScreen.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/ScreenMenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

using std::shared_ptr;

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static void applyConfig(const ConfigData& newCfg) noexcept
{
	auto globalData = GlobalConfig::INSTANCE().data();
	globalData.modelConfig = newCfg.modelConfig;
	GlobalConfig::INSTANCE().data(globalData);
	GlobalConfig::INSTANCE().save();
}

struct Resolutions {
	vector<string> names;
	vector<sfz::vec2i> resolutions;
};

static Resolutions getAvailableResolutions(ConfigData& data) noexcept
{
	struct DispMode {
		string name;
		sfz::vec2i res;
	};

	vector<DispMode> dispModes;
	SDL_DisplayMode mode;
	const int numDisplays = SDL_GetNumVideoDisplays();
	if (numDisplays < 0) {
		std::cerr << "SDL_GetNumVideoDisplays() failed: " << SDL_GetError() << std::endl;
	}
	if (data.displayIndex >= numDisplays) {
		std::cerr << "Display index " << data.displayIndex << " is invalid, max is "
		          << numDisplays << ". Resetting to 0." << std::endl;
		data.displayIndex = 0;
	}

	const int numDisplayModes = SDL_GetNumDisplayModes(data.displayIndex);
	if (numDisplayModes < 0) {
		std::cerr << "SDL_GetNumDisplayModes failed: " << SDL_GetError() << std::endl;
	}
	for (int i = 0; i < numDisplayModes; ++i) {
		mode = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
		if (SDL_GetDisplayMode(data.displayIndex, i, &mode) != 0) {
			std::cerr << "SDL_GetDisplayMode failed: " << SDL_GetError() << std::endl;
		}
		std::cout << "DisplayMode: " << mode.w << "x" << mode.h << ", " << mode.refresh_rate << "Hz, PixelFormat: " << mode.format << std::endl;
		dispModes.push_back({std::to_string(mode.w) + "x" + std::to_string(mode.h),
		                     sfz::vec2i{mode.w, mode.h}});
	}

	sfz::vec2i current{data.windowResolutionX, data.windowResolutionY};
	bool found = false;
	for (int i = 0; i < (int)dispModes.size(); ++i) {
		if (dispModes[i].res == current) {
			found = true;
			break;
		}
	}

	if (!found) {
		dispModes.push_back({std::to_string(current.x) + "x" + std::to_string(current.x),  current});
	}

	std::sort(dispModes.begin(), dispModes.end(), [](const DispMode& lhs, const DispMode& rhs) {
		return sfz::squaredLength(lhs.res) < sfz::squaredLength(rhs.res);
	});

	Resolutions temp;
	for (int i = 0; i < (int)dispModes.size(); ++i) {
		temp.names.push_back(std::move(dispModes[i].name));
		temp.resolutions.push_back(dispModes[i].res);
	}

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
	const float spacing = 5.5f;
	const float itemSpacing = 1.0f;
	const float titleHeight = 20.0f;
	const float stateAlignOffset = menuDim.x * 0.6f;
	const vec2 headingDim{menuDim.x * 0.85f, 9.0f};
	const vec2 itemDim{menuDim.x * 0.85f, 7.5f};
	float scrollListHeight = menuDim.y - titleHeight - itemDim.y - 3.0f*spacing;

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
	mResolutions = std::move(temp.resolutions);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Resolutions", std::move(temp.names), [this]() {
		sfz::vec2i current{this->cfgData.windowResolutionX, this->cfgData.windowResolutionY};
		for (int i = 0; i < (int)this->mResolutions.size(); ++i) {
			if (this->mResolutions[i] == current) return i;
		}
		return -1;
	}, [this](int choice) {
		this->cfgData.windowResolutionX = this->mResolutions[choice].x;
		this->cfgData.windowResolutionY = this->mResolutions[choice].y;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new OnOffSelector{"VSync", [this]() {
		return this->cfgData.vsync;
	}, [this]() {
		this->cfgData.vsync = !this->cfgData.vsync;
	}, stateAlignOffset}}, itemDim);

	scrollList.addSpacing(itemSpacing);
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"MSAA", {"Off", "2x", "4x", "8x", "16x"}, [this]() {
		switch (this->cfgData.msaa) {
		case 0: return 0;
		case 2: return 1;
		case 4: return 2;
		case 8: return 3;
		case 16: return 4;
		default: return -1;
		}
	}, [this](int choice) {
		switch (choice) {
		case 0: this->cfgData.msaa = 0; break;
		case 1: this->cfgData.msaa = 2; break;
		case 2: this->cfgData.msaa = 4; break;
		case 3: this->cfgData.msaa = 8; break;
		case 4: this->cfgData.msaa = 16; break;
		}
	}, stateAlignOffset}}, itemDim);

	// GameSettings
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	scrollList.addSpacing(spacing);
	scrollList.addItem(shared_ptr<BaseItem>{new TextItem{"Game Settings", HorizontalAlign::LEFT}}, headingDim);

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
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Speed increase (per obj)", {"0.000", "0.005", "0.010", "0.015", "0.020", "0.025", "0.030", "0.035", "0.040", "0.045", "0.050", "0.055", "0.060", "0.065", "0.070", "0.075", "0.080", "0.085", "0.090", "0.095", "0.100"}, [this]() {
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


	mGuiSystem.addSpacing(itemSpacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new SideSplitContainer{}}, itemDim);
	SideSplitContainer& sideSplit = *(SideSplitContainer*)mGuiSystem.items().back().get();
	
	sideSplit.setLeft(shared_ptr<BaseItem>{new Button{"Cancel", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}}, itemDim.x * 0.4f);

	sideSplit.setRight(shared_ptr<BaseItem>{new Button{"Apply", [this](Button&) {
		applyConfig(this->cfgData);
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}}, itemDim.x * 0.4);
}

// OptionsScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp OptionsScreen::update(const UpdateState& state)
{
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, screens::MIN_DRAWABLE);

	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (cancelRef) {
		applyConfig(cfgData);
		return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}
	mGuiSystem.update(data, state.delta);

	return mUpdateOp;
}

void OptionsScreen::render(const UpdateState& state)
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

	/*auto& sb = Assets::INSTANCE().spriteBatch;
	sb.begin(guiCam);
	sb.draw(mGuiSystem.bounds().position(), mGuiSystem.bounds().dimensions(), Assets::INSTANCE().TILE_FACE_REG);
	sb.draw(testBox, Assets::INSTANCE().TILE_FACE_REG);
	sb.end(0, drawableDim, Assets::INSTANCE().ATLAS_128.texture());*/

	// Draw GUI
	mGuiSystem.draw(0, drawableDim, guiCam);
}

} // namespace s3