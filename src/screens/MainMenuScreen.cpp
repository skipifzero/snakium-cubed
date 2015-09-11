#include "screens/MainMenuScreen.hpp"

#include <memory>

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/GL.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "rendering/GUIRendering.hpp"
#include "screens/GameScreen.hpp"
#include "screens/OptionsScreen.hpp"
#include "screens/ScreenMenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

using std::shared_ptr;

// MainMenuScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MainMenuScreen::MainMenuScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE}}
{
	using namespace gui;

	const vec2 menuDim = vec2{screens::MIN_DRAWABLE.x-0.1f, screens::MIN_DRAWABLE.y-0.1f};
	float spacing = 4.0f;
	float buttonWidth = menuDim.x * 0.5f;
	float logoHeight = 30.0f;
	float copyLogoHeight = 10.0f;
	float numButtons = 6.0f;
	float buttonHeight = (menuDim.y - logoHeight - copyLogoHeight - ((numButtons+1.0f)*spacing))/numButtons;
	vec2 buttonDim{buttonWidth, buttonHeight};

	auto& a = Assets::INSTANCE();

	vector<shared_ptr<BaseItem>> buttons;
	buttons.emplace_back(new Button{"Continue", [](Button& ref) {
		
	}});
	buttons.back()->disable();
	buttons.emplace_back(new Button{"New Game", [this](Button& ref) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		       shared_ptr<BaseScreen>{new GameScreen{GlobalConfig::INSTANCE().modelConfig}}};
	}});
	buttons.emplace_back(new Button{"High Scores", [](Button& ref) {
		
	}});
	buttons.back()->disable();
	buttons.emplace_back(new Button{"Options", [this](Button& ref) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new OptionsScreen{}}};
	}});
	buttons.emplace_back(new Button{"About", [](Button& ref) {
		
	}});
	buttons.back()->disable();
	buttons.emplace_back(new Button{"Exit", [this](Button& ref) {
		this->mUpdateOp = sfz::SCREEN_QUIT;
	}});

	mGuiSystem.addItem(shared_ptr<BaseItem>{new ImageItem{a.SNAKIUM_LOGO_REG, a.ATLAS_1024.texture()}}, vec2{menuDim.x, logoHeight});
	mGuiSystem.addSpacing(spacing);
	for (auto& button : buttons) {
		mGuiSystem.addItem(button, buttonDim);
		mGuiSystem.addSpacing(spacing);
	}
	mGuiSystem.addItem(shared_ptr<BaseItem>{new ImageItem{a.SKIPIFZERO_LOGO_SNAKIUM_VER_REG, a.ATLAS_1024.texture()}}, vec2{menuDim.x, copyLogoHeight});
}

// MainMenuScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp MainMenuScreen::update(const UpdateState& state)
{
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, screens::MIN_DRAWABLE);

	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (cancelRef) return sfz::SCREEN_QUIT;
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
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, screens::MIN_DRAWABLE);

	// Draw GUI
	mGuiSystem.draw(0, drawableDim, guiCam);
}

} // namespace s3