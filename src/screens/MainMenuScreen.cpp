#include "screens/MainMenuScreen.hpp"

namespace s3 {

static void renderButton(Assets& assets, const sfz::Button& b) noexcept
{
	sfz::TextureRegion* leftRegion = &assets.BUTTON_LEFT_REG;
	sfz::TextureRegion* midRegion = nullptr;
	sfz::TextureRegion* rightRegion = &assets.BUTTON_RIGHT_REG;

	if (!b.isEnabled()) {
		leftRegion = &assets.BUTTON_LEFT_DISABLED_REG;
		rightRegion = &assets.BUTTON_RIGHT_DISABLED_REG;
	} else if (b.isSelected()) {
		leftRegion = &assets.BUTTON_LEFT_TOUCHED_REG;
		midRegion = &assets.BUTTON_MIDDLE_TOUCHED_REG;
		rightRegion = &assets.BUTTON_RIGHT_TOUCHED_REG;
	}

	auto& sb = assets.mSpriteBatch;
	const auto& r = b.rect();
	sb.draw(r.pos - vec2{r.dim.x/2.0f, 0.0f}, vec2{r.dim.y}, *leftRegion);
	if (midRegion != nullptr) {
		sb.draw(r.pos, vec2{r.dim.x - r.dim.y, r.dim.y}, *midRegion);
	}
	sb.draw(r.pos + vec2{r.dim.x/2.0f, 0.0f}, vec2{r.dim.y}, *rightRegion);
}

// MainMenuScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MainMenuScreen::MainMenuScreen(sdl::Window& window, Assets& assets) noexcept
:
	mWindow{window},
	mAssets{assets},
	mNewGameButton{sfz::Rectangle{screens::MIN_DRAWABLE.x/2.0f, 80.0f, 60.0f, 20.0f}, "New Game"},
	mQuitButton{sfz::Rectangle{screens::MIN_DRAWABLE.x/2.0f, 50.0, 60.0f, 20.0f}, "Quit", [](sfz::Button& b) {b.disable();}}
{ }

// MainMenuScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp MainMenuScreen::update(const UpdateState& state)
{
	// Handle input
	for (const SDL_Event& event : state.events) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE: return sfz::SCREEN_QUIT;
			default:
				return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
				    shared_ptr<BaseScreen>{new GameScreen{mWindow, mAssets, mCfg.modelConfig}}};
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


	gl::SpriteBatch& sb = mAssets.mSpriteBatch;

	// Rendering temporary background
	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	sb.draw(screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE, mAssets.TILE_FACE_REG);
	sb.end(0, drawableDim, mAssets.ATLAS_128.texture());

	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	sb.draw(vec2{50.0f, screens::MIN_DRAWABLE.y-15.0f}, vec2{80.0f, 20.0f}, mAssets.SNAKIUM_LOGO_REG);
	sb.end(0, drawableDim, mAssets.ATLAS_1024.texture());


	// Button temp
	//sfz::Button b{sfz::Rectangle{40.0f, 40.0f, 40.0f, 20.0f}, [](auto) { std::cout << "Button clicked!\n"; }};
	//const sfz::Rectangle& r1 = b.rect();
	sb.begin(guiOffs + (guiDim/2.0f), guiDim);
	//sb.draw(r1.pos, r1.dim, mAssets.TILE_FACE_REG);

	renderButton(mAssets, mNewGameButton);
	renderButton(mAssets, mQuitButton);

	//sb.draw(r1.pos - vec2{r1.dim.x/2.0f, 0.0f}, vec2{r1.dim.y}, mAssets.BUTTON_LEFT_TOUCHED_REG);
	//sb.draw(r1.pos, vec2{r1.dim.x - r1.dim.y, r1.dim.y}, mAssets.BUTTON_MIDDLE_TOUCHED_REG);
	//sb.draw(r1.pos + vec2{r1.dim.x/2.0f, 0.0f}, vec2{r1.dim.y}, mAssets.BUTTON_RIGHT_TOUCHED_REG);

	sb.end(0, drawableDim, mAssets.ATLAS_128.texture());


	
}

void MainMenuScreen::onQuit()
{
}

void MainMenuScreen::onResize(vec2 dimensions, vec2 drawableDimensions)
{
}

} // namespace s3