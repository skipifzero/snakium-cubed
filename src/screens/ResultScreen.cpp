#include "screens/ResultScreen.hpp"

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>

#include "screens/GameScreen.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/ScreenMenuConstants.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

// ResultScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ResultScreen::ResultScreen(const ModelConfig& lastModelCfg, const Stats& results) noexcept
:
	lastModelConfig{lastModelCfg},
	results{results},
	mGuiSystem{sfz::Rectangle{screens::MIN_DRAWABLE/2.0f, screens::MIN_DRAWABLE}}
{
	using namespace gui;

	const vec2 menuDim = vec2{screens::MIN_DRAWABLE.x-0.1f, screens::MIN_DRAWABLE.y-0.1f};
	const float titleHeight = 20.0f;
	const float resultHeight = 6.0f;
	const float buttonHeight = 8.0f;
	const float numResultItems = 5.0f;
	const float bottomSpacing = 2.0f;
	const float totalSpacing = menuDim.y - titleHeight - buttonHeight - (numResultItems * resultHeight) - bottomSpacing;
	const float maxSpacing = 10.0f;
	const float spacing = std::min(totalSpacing / (numResultItems + 1.0f), maxSpacing);
	const float lastSpacing = totalSpacing - (numResultItems) * spacing;

	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{"Results"}}, vec2{menuDim.x, titleHeight});

	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{std::string{"Score: "} + std::to_string(results.score)}}, vec2{menuDim.x, resultHeight});

	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{std::string{"Objects eaten: "} +std::to_string(results.objectsEaten)}}, vec2{menuDim.x, resultHeight});

	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{std::string{"Bonus objects eaten: "} +std::to_string(results.bonusObjectsEaten)}}, vec2{menuDim.x, resultHeight});

	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{std::string{"Missed bonus objects: "} +std::to_string(results.missedBonusObjects)}}, vec2{menuDim.x, resultHeight});

	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{std::string{"Number of dives: "} +std::to_string(results.numberOfDives)}}, vec2{menuDim.x, resultHeight});

	mGuiSystem.addSpacing(lastSpacing);
	mRetryExitCon = shared_ptr<BaseItem>{new SideSplitContainer{}};
	mGuiSystem.addItem(mRetryExitCon, vec2{menuDim.x, buttonHeight});
	SideSplitContainer& sideSplit = *(SideSplitContainer*)mGuiSystem.items().back().get();

	mRetryButton = shared_ptr<BaseItem>{new Button{"Retry", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                  shared_ptr<BaseScreen>{new GameScreen{this->lastModelConfig}}};
	}}};
	sideSplit.setLeft(mRetryButton, menuDim.x * 0.4f);

	mExitButton = shared_ptr<BaseItem>{new Button{"Main Menu", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                  shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}};
	sideSplit.setRight(mExitButton, menuDim.x * 0.4f);
}

// ResultScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp ResultScreen::update(UpdateState& state)
{
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, screens::MIN_DRAWABLE);

	int32_t ctrlId = getFirstController(state);
	bool cancelRef;
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, &cancelRef);
	if (cancelRef) {
		return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                std::shared_ptr<sfz::BaseScreen>{new MainMenuScreen{}}};
	}
	mGuiSystem.update(data, state.delta);

	return mUpdateOp;
}

void ResultScreen::render(UpdateState& state)
{
	// Clearing screen
	glClearColor(screens::BG_COLOR.x, screens::BG_COLOR.y, screens::BG_COLOR.z, 1.0f);
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