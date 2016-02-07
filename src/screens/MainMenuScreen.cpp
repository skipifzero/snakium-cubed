#include "screens/MainMenuScreen.hpp"

#include <memory>

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "screens/ModeSelectScreen.hpp"
#include "screens/OptionsScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

using std::shared_ptr;

// MainMenuScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

MainMenuScreen::MainMenuScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;

	float spacing = 4.0f;
	float buttonWidth = MENU_DIM.x * 0.55f;
	float numButtons = 6.0f;
	float buttonHeight = (MENU_DIM.y - TITLE_HEIGHT - NAVBAR_HEIGHT - MENU_TOP_PADDING - MENU_BOTTOM_PADDING - ((numButtons+1.0f)*spacing))/numButtons;
	vec2 buttonDim{buttonWidth, buttonHeight};

	auto& a = Assets::INSTANCE();

	vector<shared_ptr<BaseItem>> buttons;
	buttons.emplace_back(new Button{"Continue", [](Button& ref) {
		
	}});
	buttons.back()->disable();
	buttons.emplace_back(new Button{"New Game", [this](Button& ref) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		       shared_ptr<BaseScreen>{new ModeSelectScreen()}};
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

	mGuiSystem.addSpacing(MENU_TOP_PADDING);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new ImageItem{a.SNAKIUM_LOGO_REG, a.ATLAS_1024.texture()}}, vec2{MENU_DIM.x, TITLE_HEIGHT});
	mGuiSystem.addSpacing(spacing);
	for (auto& button : buttons) {
		mGuiSystem.addItem(button, buttonDim);
		mGuiSystem.addSpacing(spacing);
	}
	mGuiSystem.addItem(shared_ptr<BaseItem>{new ImageItem{a.SKIPIFZERO_LOGO_SNAKIUM_VER_REG, a.ATLAS_1024.texture()}}, vec2{MENU_DIM.x, NAVBAR_HEIGHT});
}

// MainMenuScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp MainMenuScreen::update(UpdateState& state)
{
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (cancelRef) return sfz::SCREEN_QUIT;
	mGuiSystem.update(data, state.delta);

	return mUpdateOp;
}

void MainMenuScreen::render(UpdateState& state)
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

} // namespace s3