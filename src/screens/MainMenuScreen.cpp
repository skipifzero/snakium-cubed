#include "screens/MainMenuScreen.hpp"

#include <memory>

#include <sfz/GL.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "rendering/GUIRendering.hpp"
#include "screens/GameScreen.hpp"
#include "screens/ScreenMenuConstants.hpp"

namespace s3 {

using std::shared_ptr;

// MainMenuScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MainMenuScreen::MainMenuScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE}}
{
	auto& a = Assets::INSTANCE();

	mGuiSystem.addItem(shared_ptr<gui::BaseItem>{new gui::ImageItem{a.SNAKIUM_LOGO_REG, a.ATLAS_1024.texture()}}, vec2{screens::MIN_DRAWABLE.x, 30.0f});
	mGuiSystem.addSpacing(10.0f);
	mGuiSystem.addItem(shared_ptr<gui::BaseItem>{new gui::Button{"Button1", [](gui::Button& ref) { if (ref.isEnabled()) ref.disable(); else ref.enable(); }}}, vec2{60.0f, 15.0f});
	mGuiSystem.addSpacing(6.0f);
	mGuiSystem.addItem(shared_ptr<gui::BaseItem>{new gui::Button{"Button2", [](gui::Button& ref) { if (ref.isEnabled()) ref.disable(); else ref.enable(); }}}, vec2{60.0f, 15.0f});
	mGuiSystem.addSpacing(6.0f);
	mGuiSystem.addItem(shared_ptr<gui::BaseItem>{new gui::Button{"Button3", [](gui::Button& ref) { if (ref.isEnabled()) ref.disable(); else ref.enable(); }}}, vec2{60.0f, 15.0f});
}

// MainMenuScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp MainMenuScreen::update(const UpdateState& state)
{
	Assets& assets = Assets::INSTANCE();
	GlobalConfig& cfg = GlobalConfig::INSTANCE();

	gui::KeyInput guiKeyInput = gui::KeyInput::NONE;

	// Handle input
	for (const SDL_Event& event : state.events) {
		switch (event.type) {
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE: return sfz::SCREEN_QUIT;
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

			case 'n':
			case 'N':
				return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
					shared_ptr<BaseScreen>{new GameScreen{state.window, cfg.modelConfig}}};
			}
		}
	}

	const vec2 drawableDim = state.window.drawableDimensions();
	const vec2 guiDim = screens::guiDimensions(drawableDim);
	const vec2 guiOffs = screens::guiOffset(guiDim);

	auto scaledMouse = state.rawMouse.scaleMouse(guiDim, guiOffs);

	// GUI system temp
	gui::InputData data;
	data.pointerMoved = scaledMouse.motion != vec2{0.0f, 0.0f};
	data.pointerPos = scaledMouse.position;
	data.pointerState = scaledMouse.leftButton;
	data.scrollWheel = scaledMouse.wheel;
	data.key = guiKeyInput;
	mGuiSystem.update(data);

	return mUpdateOp;
}

void MainMenuScreen::render(const UpdateState& state)
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

	// Draw GUI
	mGuiSystem.draw(0, drawableDim, guiOffs + (guiDim/2.0f), guiDim);
}

} // namespace s3