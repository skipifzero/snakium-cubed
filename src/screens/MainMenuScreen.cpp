#include "screens/MainMenuScreen.hpp"

#include <memory>

#include <sfz/GL.hpp>

#include "Assets.hpp"
#include "GlobalConfig.hpp"
#include "rendering/GUIRendering.hpp"
#include "screens/GameScreen.hpp"
#include "screens/ScreenMenuConstants.hpp"

namespace s3 {

// MainMenuScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MainMenuScreen::MainMenuScreen(sdl::Window& window) noexcept
:
	mWindow{window},
	mNewGameButton{sfz::Rectangle{screens::MIN_DRAWABLE.x/2.0f, 80.0f, 60.0f, 20.0f}, "New Game"},
	mQuitButton{sfz::Rectangle{screens::MIN_DRAWABLE.x/2.0f, 50.0, 60.0f, 20.0f}, "Quit", [](sfz::Button& b) {b.disable();}}
{ }

// MainMenuScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp MainMenuScreen::update(const UpdateState& state)
{
	Assets& assets = Assets::INSTANCE();
	GlobalConfig& cfg = GlobalConfig::INSTANCE();

	// Handle input
	for (const SDL_Event& event : state.events) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE: return sfz::SCREEN_QUIT;
			default:
				return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
				    std::shared_ptr<BaseScreen>{new GameScreen{mWindow, cfg.modelConfig}}};
			}
			break;
		}
	}

	const vec2 drawableDim = mWindow.drawableDimensions();
	const vec2 guiDim = screens::guiDimensions(drawableDim);
	const vec2 guiOffs = screens::guiOffset(guiDim);

	auto scaledMouse = state.rawMouse.scaleMouse(guiDim, guiOffs);
	mNewGameButton.update(scaledMouse.position, scaledMouse.leftButton == sdl::ButtonState::UP);
	mQuitButton.update(scaledMouse.position, scaledMouse.leftButton == sdl::ButtonState::UP);

	return sfz::SCREEN_NO_OP;
}

void MainMenuScreen::render(const UpdateState& state)
{
	Assets& assets = Assets::INSTANCE();

	const vec2 drawableDim = mWindow.drawableDimensions();
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
	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	sb.draw(screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE, assets.TILE_FACE_REG);
	sb.end(0, drawableDim, assets.ATLAS_128.texture());

	// Render logo
	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	sb.draw(vec2{ 50.0f, screens::MIN_DRAWABLE.y-15.0f }, vec2{ 80.0f, 20.0f }, assets.SNAKIUM_LOGO_REG);
	sb.end(0, drawableDim, assets.ATLAS_1024.texture());

	// Render buttons
	renderButton(mNewGameButton, drawableDim, guiDim, guiOffs);
	renderButton(mQuitButton, drawableDim, guiDim, guiOffs);
}

} // namespace s3