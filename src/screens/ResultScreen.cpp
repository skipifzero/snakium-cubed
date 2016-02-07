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
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;

	const float subTitleHeight = 8.0f;
	const float resultHeight = 6.0f;
	const float subResultHeight = 4.0f;

	const float numSubTitles = 2.0f;
	const float numResultItems = 2.0f;
	const float numSubResultItems = 6.0f;
	
	const float spacing = 4.5f;
	const float subSpacing = 2.0f;
	const float totalSpacing = MENU_DIM.y - TITLE_HEIGHT - NAVBAR_HEIGHT - (numSubTitles * subTitleHeight) - (numResultItems * resultHeight) - (numSubResultItems * subResultHeight) - MENU_TOP_PADDING - MENU_BOTTOM_PADDING;
	const float lastSpacing = totalSpacing - (numSubTitles) * spacing - (numResultItems + numSubResultItems) * subSpacing;

	const float align = MENU_DIM.x / 2.0f;

	mGuiSystem.addSpacing(MENU_TOP_PADDING);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{"Results"}}, vec2{MENU_DIM.x, TITLE_HEIGHT});
	char tmp[256];

	mGuiSystem.addSpacing(spacing);
	std::snprintf(tmp, sizeof(tmp), "%i", totalScore(results, lastModelCfg));
	mGuiSystem.addItem(shared_ptr<BaseItem>{new DualTextItem{"Score: ", tmp, align, gl::HorizontalAlign::RIGHT}}, vec2{MENU_DIM.x, subTitleHeight});

	mGuiSystem.addSpacing(subSpacing);
	std::snprintf(tmp, sizeof(tmp), "%i x %i points", results.objectsEaten, lastModelCfg.objectValue);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new DualTextItem{"Objects: ", tmp, align, gl::HorizontalAlign::RIGHT}}, vec2{MENU_DIM.x, resultHeight});

	mGuiSystem.addSpacing(subSpacing);
	std::snprintf(tmp, sizeof(tmp), "%i x %i points", results.objectsEarly, lastModelCfg.objectEarlyBonus);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new DualTextItem{"early bonus: ", tmp, align, gl::HorizontalAlign::RIGHT}}, vec2{MENU_DIM.x, subResultHeight});

	mGuiSystem.addSpacing(subSpacing);
	std::snprintf(tmp, sizeof(tmp), "%i x %i points", results.objectsShift, lastModelCfg.objectShiftBonus);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new DualTextItem{"shift bonus: ", tmp, align, gl::HorizontalAlign::RIGHT}}, vec2{MENU_DIM.x, subResultHeight});

	mGuiSystem.addSpacing(subSpacing);
	std::snprintf(tmp, sizeof(tmp), "%i x %i points", results.bonusObjectsEaten, lastModelCfg.bonusObjectValue);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new DualTextItem{"Bonus objects: ", tmp, align, gl::HorizontalAlign::RIGHT}}, vec2{MENU_DIM.x, resultHeight});

	mGuiSystem.addSpacing(subSpacing);
	std::snprintf(tmp, sizeof(tmp), "%i x %i points", results.bonusObjectsShift, lastModelCfg.bonusObjectShiftBonus);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new DualTextItem{"shift bonus: ", tmp, align, gl::HorizontalAlign::RIGHT}}, vec2{MENU_DIM.x, subResultHeight});

	mGuiSystem.addSpacing(subSpacing);
	std::snprintf(tmp, sizeof(tmp), "%i :(", results.bonusObjectsMissed);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new DualTextItem{"missed: ", tmp, align, gl::HorizontalAlign::RIGHT}}, vec2{MENU_DIM.x, subResultHeight});

	mGuiSystem.addSpacing(spacing);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new TextItem{"Misc stats"}}, vec2{MENU_DIM.x, subTitleHeight});

	mGuiSystem.addSpacing(subSpacing);
	std::snprintf(tmp, sizeof(tmp), "%i", results.tilesTraversed);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new DualTextItem{"Tiles traversed: ", tmp, align, gl::HorizontalAlign::RIGHT}}, vec2{MENU_DIM.x, subResultHeight});

	mGuiSystem.addSpacing(subSpacing);
	std::snprintf(tmp, sizeof(tmp), "%i", results.numberOfShifts);
	mGuiSystem.addItem(shared_ptr<BaseItem>{new DualTextItem{"Shifts: ", tmp, align, gl::HorizontalAlign::RIGHT}}, vec2{MENU_DIM.x, subResultHeight});


	mGuiSystem.addSpacing(lastSpacing);
	mRetryExitCon = shared_ptr<BaseItem>{new SideSplitContainer{}};
	mGuiSystem.addItem(mRetryExitCon, vec2{MENU_DIM.x, NAVBAR_HEIGHT});
	SideSplitContainer& sideSplit = *(SideSplitContainer*)mGuiSystem.items().back().get();

	mRetryButton = shared_ptr<BaseItem>{new Button{"Retry", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                  shared_ptr<BaseScreen>{new GameScreen{this->lastModelConfig}}};
	}}};
	sideSplit.setLeft(mRetryButton, MENU_DIM.x * 0.4f);

	mExitButton = shared_ptr<BaseItem>{new Button{"Main Menu", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                  shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}};
	sideSplit.setRight(mExitButton, MENU_DIM.x * 0.4f);
}

// ResultScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp ResultScreen::update(UpdateState& state)
{
	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

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