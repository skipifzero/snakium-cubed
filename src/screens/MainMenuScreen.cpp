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
	using namespace gui;

	vec2 menuDim = screens::MIN_DRAWABLE;
	menuDim.x -= 0.1f;
	menuDim.y -= 0.1f;
	float spacing = 4.0f;
	float buttonWidth = menuDim.x * 0.5f;
	float logoHeight = 30.0f;
	float copyLogoHeight = 10.0f;
	float numButtons = 6.0f;
	float buttonHeight = (menuDim.y - logoHeight - copyLogoHeight - ((numButtons+1.0f)*spacing))/numButtons;
	vec2 buttonDim{buttonWidth, buttonHeight};

	auto& a = Assets::INSTANCE();

	mGuiSystem.addItem(shared_ptr<BaseItem>{new ImageItem{a.SNAKIUM_LOGO_REG, a.ATLAS_1024.texture()}}, vec2{menuDim.x, logoHeight});
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new Button{"Continue", [](Button& ref) { }}}, buttonDim);
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new Button("New Game", [this](Button& ref) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN, shared_ptr<BaseScreen>{new GameScreen{GlobalConfig::INSTANCE().modelConfig}}};
	})}, buttonDim);
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new Button{"High Scores", [](Button& ref) {}}}, buttonDim);
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new Button{"Options", [](Button& ref) { }}}, buttonDim);
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new Button{"About", [](Button& ref) {}}}, buttonDim);
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new Button("Exit", [this](Button& ref) {
		this->mUpdateOp = sfz::SCREEN_QUIT;
	})}, buttonDim);
	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new ImageItem{a.SKIPIFZERO_LOGO_SNAKIUM_VER_REG, a.ATLAS_1024.texture()}}, vec2{menuDim.x, copyLogoHeight});
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
			//case SDLK_ESCAPE: return sfz::SCREEN_QUIT;
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

	const vec2 drawableDim = state.window.drawableDimensions();
	const vec2 guiDim = screens::guiDimensions(drawableDim);
	const vec2 guiOffs = screens::guiOffset(guiDim);

	auto scaledMouse = state.rawMouse.scaleMouse(guiOffs + (guiDim/2.0f), guiDim);

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