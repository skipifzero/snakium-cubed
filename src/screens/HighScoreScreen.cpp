#include "screens/HighScoreScreen.hpp"

#include "sfz/gl/OpenGL.hpp"

#include "rendering/Assets.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

using std::shared_ptr;

// HighScoreScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

HighScoreScreen::HighScoreScreen() noexcept
:
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;
	auto& a = Assets::INSTANCE();

	bool hasScores = loadScores(mScores);
	const float buttonWidth = MENU_DIM.x * 0.4f;
	float restPadding;
	if (hasScores) restPadding = calcRestPadding(0.0f, 0.0f, 0.0f, 0.0f);
	else restPadding = calcRestPadding(0.0f, 0.0f, 2.0f, 1.0f);

	// Title
	addTitle(mGuiSystem, new TextItem{"High Scores"});
	
	// Error text if high scores couldn't be loaded
	if (!hasScores) {
		addStandardPadding(mGuiSystem);
		addHeading3(mGuiSystem, new TextItem{"Could not load high scores file"});
		addHeading3(mGuiSystem, new TextItem{"(this is normal if no high scores has been made)"});
		addNavbar(mGuiSystem, new Button{"Back", [this](Button&) {
			this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
			                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
		}}, restPadding, buttonWidth);
		return;
	}
}

// HighScoreScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp HighScoreScreen::update(UpdateState& state)
{
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

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

void HighScoreScreen::render(UpdateState& state)
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