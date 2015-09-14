#include "screens/OptionsScreen.hpp"

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

// OptionsScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

OptionsScreen::OptionsScreen() noexcept
:
	cfgData{GlobalConfig::INSTANCE().data()},
	mGuiSystem{sfz::Rectangle{screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE}}
{
	using namespace gui;

	const vec2 menuDim = vec2{screens::MIN_DRAWABLE.x-0.1f, screens::MIN_DRAWABLE.y-0.1f};
	const float spacing = 5.5f;
	const float titleHeight = 20.0f;
	const vec2 itemDim{menuDim.x * 0.8f, 10.0f};
	float scrollListHeight = menuDim.y - titleHeight - itemDim.y - 3.0f*spacing;

	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{"Options"}}, vec2{menuDim.x, titleHeight});
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new ScrollListContainer{7.5f}},
	                   vec2{menuDim.x, scrollListHeight});
	
	ScrollListContainer& scrollList = *(ScrollListContainer*)mGuiSystem.items().back().get();
	scrollList.addItem(shared_ptr<BaseItem>{new TextItem{"Graphics", HorizontalAlign::LEFT}}, itemDim);
	scrollList.addSpacing(spacing);
	scrollList.addItem(shared_ptr<BaseItem>{new OnOffSelector{"Fullscreen", [this]() {
		return this->cfgData.fullscreen;
	}, [this]() {
		this->cfgData.fullscreen = !this->cfgData.fullscreen;
	}}}, itemDim);
	/*
	scrollList.addSpacing(spacing);
	strings = {"320x240", "640x480", "1024x600", "1280x720", "1600x900", "1920x1080", "2560x1440"};
	scrollList.addItem(shared_ptr<BaseItem>{new MultiChoiceSelector{"Resolution", strings, [this]() {
		return this->testInt;
	}, [this](int choice) {
		this->testInt = choice;
	}}}, vec2{buttonWidth, buttonHeight});
	*/
	scrollList.addSpacing(spacing);
	scrollList.addItem(shared_ptr<BaseItem>{new OnOffSelector{"VSync", [this]() {
		return this->cfgData.vsync;
	}, [this]() {
		this->cfgData.vsync = !this->cfgData.vsync;
	}}}, itemDim);

	scrollList.addSpacing(spacing);
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
	}}}, itemDim);

	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new Button{"Back", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}}, itemDim);
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