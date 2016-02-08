#include "screens/OptionsGraphicsScreen.hpp"

#include <algorithm>
#include <memory>

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>
#include <sfz/math/MathHelpers.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "screens/GameScreen.hpp"
#include "screens/OptionsScreen.hpp"
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

// OptionsGraphicsScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

OptionsGraphicsScreen::OptionsGraphicsScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;
	cfgData = GlobalConfig::INSTANCE().data();
	mYResolutions = availableVerticalResolutions();

	// Constants
	const float stateAlignOffset = MENU_DIM.x * 0.535f;

	// Title, scrollList and navbar
	addTitle(mGuiSystem, new TextItem{"Graphics"});
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

	// ScrollList: Graphics
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Window", gl::HorizontalAlign::LEFT}});

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

	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Internal resolution", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new OnOffSelector{"Native resolution", [this]() {
		return this->cfgData.nativeInternalRes;
	}, [this]() {
		this->cfgData.nativeInternalRes = !this->cfgData.nativeInternalRes;
		if (this->cfgData.nativeInternalRes) {
			this->mInternalResMultiChoicePtr->disable();
		} else {
			this->mInternalResMultiChoicePtr->enable();
		}
		this->mDrawableDim = vec2{-1.0f};
	}, stateAlignOffset}});

	mInternalResMultiChoicePtr = shared_ptr<BaseItem>{new MultiChoiceSelector{"Internal y-resolution", {}, [this]() {
		int val = this->cfgData.internalResolutionY;
		auto itr = std::find(this->mYResolutions.begin(), this->mYResolutions.end(), val);
		if (itr == this->mYResolutions.end()) return -1;
		return static_cast<int>(itr - this->mYResolutions.begin());
	}, [this](int choice) {
		this->cfgData.internalResolutionY = this->mYResolutions[choice];
		this->mDrawableDim = vec2{-1.0f};
	}, stateAlignOffset}};
	if (this->cfgData.nativeInternalRes) {
		mInternalResMultiChoicePtr->disable();
	} else {
		mInternalResMultiChoicePtr->enable();
	}
	addHeading3(scrollList, mInternalResMultiChoicePtr);

	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Secondary resolutions", gl::HorizontalAlign::LEFT}});

	mBlurResMultiChoicePtr = shared_ptr<BaseItem>{new MultiChoiceSelector{"Emissive blur resolution", {}, [this]() {
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
	
	mSpotlightResMultiChoicePtr = shared_ptr<BaseItem>{new MultiChoiceSelector{"Spotlight resolution", {}, [this]() {
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
	
	mLightShaftResMultiChoicePtr = shared_ptr<BaseItem>{new MultiChoiceSelector{"Volumetric lighting resolution", {}, [this]() {
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
	
	addStandardPadding(scrollList);
	addHeading2(scrollList, shared_ptr<BaseItem>{new TextItem{"Misc", gl::HorizontalAlign::LEFT}});

	addHeading3(scrollList, shared_ptr<BaseItem>{new MultiChoiceSelector{"Scaling algorithm", {"Nearest", "Bilinear", "2x2 Nearest", "2x2 Bilinear", "4x4 Nearest", "4x4 Bilinear", "Bicubic Bspline", "Lanczos-2", "Lanczos-3"}, [this]() {
		return this->cfgData.scalingAlgorithm;
	}, [this](int choice) {
		this->cfgData.scalingAlgorithm = choice;
	}, stateAlignOffset}});
}

// OptionsGraphicsScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp OptionsGraphicsScreen::update(UpdateState& state)
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
			                shared_ptr<BaseScreen>{new OptionsScreen{}}};
		}

		// Awful, awful hack to select apply button if back is pressed when changes have been made.
		vec2 oldPointerPos = data.pointerPos;
		data.pointerPos = mGuiSystem.bounds().position() + mCancelApplyCon->offset + mApplyButton->offset;
		data.pointerMotion += (data.pointerPos - oldPointerPos);
	}
	mGuiSystem.update(data, state.delta);

	return mUpdateOp;
}

void OptionsGraphicsScreen::render(UpdateState& state)
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

void OptionsGraphicsScreen::onResize(vec2 windowDimensions, vec2 drawableDimensions)
{
	mDrawableDim = drawableDimensions;
	updateResolutionStrings();
}

// OptionsGraphicsScreen: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void OptionsGraphicsScreen::applyConfig() noexcept
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

void OptionsGraphicsScreen::updateResolutionFactors() noexcept
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
	int internalResY;
	if (cfgData.nativeInternalRes) {
		internalResY = (int)mDrawableDim.y;
	} else {
		internalResY = cfgData.internalResolutionY;
	}
	vec2i internalRes{(int)std::round(internalResY * aspect), internalResY};
	for (float factor = 0.05f; factor <= 4.0f; factor += 0.05) {
		std::snprintf(buffer, 128, "%ix%i (%.2f)", int(std::round(internalRes.x * factor)), 
		              int(std::round(internalRes.y * factor)), factor);
		mSecondaryResFactorStrs.push_back(buffer);
	}
}

void OptionsGraphicsScreen::updateResolutionStrings() noexcept
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