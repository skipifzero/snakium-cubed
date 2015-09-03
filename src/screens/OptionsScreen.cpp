#include "screens/OptionsScreen.hpp"

#include <memory>

#include <sfz/GL.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "rendering/GUIRendering.hpp"
#include "screens/GameScreen.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/ScreenMenuConstants.hpp"

namespace s3 {

using std::shared_ptr;

// OptionsScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

OptionsScreen::OptionsScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE}}
{
	using namespace gui;

	const vec2 menuDim = vec2{screens::MIN_DRAWABLE.x-0.1f, screens::MIN_DRAWABLE.y-0.1f};
	float spacing = 5.0f;
	float titleHeight = 20.0f;
	float buttonWidth = menuDim.x * 0.5f;
	float buttonHeight = 9.0f;
	float scrollListHeight = menuDim.y - titleHeight - buttonHeight - 3.0f*spacing;

	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{"Options"}}, vec2{menuDim.x, titleHeight});
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new ScrollListContainer{}}, vec2{menuDim.x, scrollListHeight});
	
	ScrollListContainer& scrollList = *(ScrollListContainer*)mGuiSystem.items().back().get();
	scrollList.addItem(shared_ptr<BaseItem>{new Button{"Button 1", [](Button& ref) {
		ref.disable();
	}}}, vec2{buttonWidth, buttonHeight});
	scrollList.addSpacing(spacing);
	scrollList.addItem(shared_ptr<BaseItem>{new Button{"Button 2", [](Button& ref) {
		ref.disable();
	}}}, vec2{buttonWidth, buttonHeight});
	scrollList.addSpacing(spacing);
	scrollList.addItem(shared_ptr<BaseItem>{new Button{"Button 3", [](Button& ref) {
		ref.disable();
	}}}, vec2{buttonWidth, buttonHeight});
	scrollList.addSpacing(spacing);
	scrollList.addItem(shared_ptr<BaseItem>{new Button{"Button 4", [](Button& ref) {
		ref.disable();
	}}}, vec2{buttonWidth, buttonHeight});
	scrollList.addSpacing(spacing);


	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new Button{"Back", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}}, vec2{buttonWidth, buttonHeight});
}

// OptionsScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp OptionsScreen::update(const UpdateState& state)
{
	Assets& assets = Assets::INSTANCE();
	GlobalConfig& cfg = GlobalConfig::INSTANCE();

	gui::KeyInput guiKeyInput = gui::KeyInput::NONE;

	// Handle input
	for (const SDL_Event& event : state.events) {
		switch (event.type) {
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE: return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			                                  shared_ptr<BaseScreen>{new MainMenuScreen{}}};
			case SDLK_UP:
			case 'w':
			case 'W':
				guiKeyInput = gui::KeyInput::UP;
				break;
			case SDLK_DOWN:
			case 's':
			case 'S':
				guiKeyInput = gui::KeyInput::DOWN;
				break;
			case SDLK_LEFT:
			case 'a':
			case 'A':
				guiKeyInput = gui::KeyInput::LEFT;
				break;
			case SDLK_RIGHT:
			case 'd':
			case 'D':
				guiKeyInput = gui::KeyInput::DOWN;
				break;
			case SDLK_RETURN:
			case SDLK_SPACE:
				guiKeyInput = gui::KeyInput::ACTIVATE;
				break;
			}
		}
	}

	std::cout << "scrollWheel: " << state.rawMouse.wheel << std::endl;

	const vec2 drawableDim = state.window.drawableDimensions();
	const vec2 guiDim = screens::guiDimensions(drawableDim);
	const vec2 guiOffs = screens::guiOffset(guiDim);

	auto scaledMouse = state.rawMouse.scaleMouse(guiOffs + (guiDim/2.0f), guiDim);

	// GUI system temp
	gui::InputData data;
	data.pointerPos = scaledMouse.position;
	data.pointerMotion = scaledMouse.motion;
	data.pointerState = scaledMouse.leftButton;
	data.scrollWheel = scaledMouse.wheel;
	data.key = guiKeyInput;
	mGuiSystem.update(data);

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
	const vec2 guiDim = screens::guiDimensions(drawableDim);
	const vec2 guiOffs = screens::guiOffset(guiDim);

	auto& sb = Assets::INSTANCE().spriteBatch;
	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	sb.draw(mGuiSystem.bounds().pos, mGuiSystem.bounds().dim, Assets::INSTANCE().TILE_FACE_REG);
	sb.end(0, drawableDim, Assets::INSTANCE().ATLAS_128.texture());

	// Draw GUI
	mGuiSystem.draw(0, drawableDim, guiOffs + (guiDim/2.0f), guiDim);
}

} // namespace s3