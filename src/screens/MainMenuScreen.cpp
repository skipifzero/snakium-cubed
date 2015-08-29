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
	//mNewGameButton{sfz::Rectangle{screens::MIN_DRAWABLE.x/2.0f, 80.0f, 60.0f, 20.0f}, "New Game"},
	//mQuitButton{sfz::Rectangle{screens::MIN_DRAWABLE.x/2.0f, 50.0, 60.0f, 20.0f}, "Quit", [](sfz::Button& b) {b.disable();}}
	mGuiSystem{sfz::Rectangle{screens::MIN_DRAWABLE.x/2.0f, (screens::MIN_DRAWABLE.y-30.0f)/2.0f, screens::MIN_DRAWABLE.x, screens::MIN_DRAWABLE.y-30.0f}}
{
	mGuiSystem.addItem(shared_ptr<gui::BaseItem>{new gui::Button{"Button1", [](gui::Button& ref) { ref.mText = ref.mText + "h"; }}}, vec2{screens::MIN_DRAWABLE.x, 15.0f});
	mGuiSystem.addSpacing(10.0f);
	mGuiSystem.addItem(shared_ptr<gui::BaseItem>{new gui::Button{"Button2", [](gui::Button& ref) { ref.mText = ref.mText + "h"; }}}, vec2{80.0f, 30.0f});
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
	//mNewGameButton.update(scaledMouse.position, scaledMouse.leftButton == sdl::ButtonState::UP);
	//mQuitButton.update(scaledMouse.position, scaledMouse.leftButton == sdl::ButtonState::UP);


	// GUI system temp
	gui::InputData data;
	data.pointerMoved = scaledMouse.motion != vec2{0.0f, 0.0f};
	data.pointerPos = scaledMouse.position;
	data.pointerState = scaledMouse.leftButton;
	data.scrollWheel = scaledMouse.wheel;
	data.key = guiKeyInput;
	mGuiSystem.update(data);

	return sfz::SCREEN_NO_OP;
}

void MainMenuScreen::render(const UpdateState& state)
{
	Assets& assets = Assets::INSTANCE();

	const vec2 drawableDim = state.window.drawableDimensions();
	const float aspect = drawableDim.x/drawableDim.y;
	const vec2 guiDim = screens::guiDimensions(drawableDim);
	const vec2 guiOffs = screens::guiOffset(guiDim);

	// Clearing screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gl::SpriteBatch& sb = assets.spriteBatch;

	// Render temporary background
	/*sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	sb.draw(screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE, assets.TILE_FACE_REG);
	sb.end(0, drawableDim, assets.ATLAS_128.texture());*/

	// Render logo
	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	sb.draw(vec2{50.0f, screens::MIN_DRAWABLE.y-15.0f}, vec2{80.0f, 20.0f}, assets.SNAKIUM_LOGO_REG);
	sb.end(0, drawableDim, assets.ATLAS_1024.texture());

	mGuiSystem.draw(drawableDim, guiOffs + (guiDim/2.0f), guiDim);

	// Render buttons
	//renderButton(mNewGameButton, drawableDim, guiDim, guiOffs);
	//renderButton(mQuitButton, drawableDim, guiDim, guiOffs);
}

} // namespace s3