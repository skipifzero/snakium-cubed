#include "screens/MainMenuScreen.hpp"

namespace s3 {

// MainMenuScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MainMenuScreen::MainMenuScreen(sdl::Window& window, Assets& assets) noexcept
:
	mWindow{window},
	mAssets{assets}
{ }

// MainMenuScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ScreenUpdateOp MainMenuScreen::update(const vector<SDL_Event>& events,
	                              const unordered_map<int32_t, sdl::GameController>& controllers,
	                              float delta)
{

	// Handle input
	for (const SDL_Event& event : events) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE: return ScreenUpdateOp{sfz::ScreenUpdateOpType::QUIT_APPLICATION};
			default:
				return ScreenUpdateOp{sfz::ScreenUpdateOpType::SWITCH_SCREEN,
				    shared_ptr<BaseScreen>{new GameScreen{mWindow, mAssets, mCfg.modelConfig}}};
			}
			break;
		}
	}

	return ScreenUpdateOp{sfz::ScreenUpdateOpType::NO_OPERATION};
}

void MainMenuScreen::render(float delta)
{
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

	// Rendering.. something
	gl::SpriteBatch& sb = mAssets.mSpriteBatch;
	sb.begin(guiOffs + (guiDim/2.0f), guiDim);

	sb.draw(vec2{50.0f, screens::MIN_DRAWABLE.y-15.0f}, vec2{80.0f, 20.0f}, mAssets.SNAKIUM_LOGO_REG);

	sb.end(0, drawableDim, mAssets.ATLAS_1024.texture());


	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	sb.draw(screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE, mAssets.TILE_FACE_REG);
	sb.end(0, drawableDim, mAssets.ATLAS_128.texture());
}

void MainMenuScreen::onQuit()
{
}

void MainMenuScreen::onResize(vec2 dimensions)
{
}

} // namespace s3