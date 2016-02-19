#include "screens/ResultScreen.hpp"

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>

#include "screens/GameScreen.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/MenuConstants.hpp"
#include "screens/ScreenUtils.hpp"
#include "ScoreManagement.hpp"

namespace s3 {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool isValidScoreConfig(const ModelConfig& cfg) noexcept
{
	return cfg == STANDARD_CONFIG || cfg == LARGE_CONFIG || cfg == GIANT_CONFIG;
}

ScoreConfigType cfgToType(const ModelConfig& cfg) noexcept
{
	if (cfg == STANDARD_CONFIG) return ScoreConfigType::STANDARD;
	if (cfg == LARGE_CONFIG) return ScoreConfigType::LARGE;
	if (cfg == GIANT_CONFIG) return ScoreConfigType::GIANT;
	sfz_error("Logic error! Impossible config!");
}

// ResultScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ResultScreen::ResultScreen(const ModelConfig& lastModelCfg, const Stats& results) noexcept
:
	lastModelConfig{lastModelCfg},
	results{results},
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;
	char tmp[256];

	bool scoresActive = isValidScoreConfig(lastModelCfg);
	ScoreBundle scores = createEmptyScoreBundle();
	loadScores(scores);
	int rank = -1;
	if (scoresActive) {
		char name[SCORE_NAME_LENGTH+1] = "herp_name"; 
		rank = tryAddScoreToBundle(scores, cfgToType(lastModelCfg), results, name);
		if (rank != -1) {
			if (writeScores(scores)) {
				std::cout << "Wrote scores to file!\n";
			}
		}
	}

	const float alignLine = MENU_DIM.x * 0.5f;
	const float restPadding = calcRestPadding(2.0f, 2.0f, 9.0f, 1.0f);
	const gl::HorizontalAlign hAlign = gl::HorizontalAlign::RIGHT;

	// Title
	addTitle(mGuiSystem, new TextItem{"Results"});

	// Content
	std::snprintf(tmp, sizeof(tmp), "Score: %i", totalScore(results, lastModelCfg));
	addHeading1(mGuiSystem, new TextItem{tmp});


	addHeading2(mGuiSystem, new TextItem{"Objects"});

	std::snprintf(tmp, sizeof(tmp), "%i  (x%i pts)", results.objectsEaten, lastModelCfg.objectValue);
	addHeading3(mGuiSystem, new DualTextItem{"Eaten: ", tmp, alignLine, hAlign});

	std::snprintf(tmp, sizeof(tmp), "%i  (x%i pts)", results.objectsEarly, lastModelCfg.objectEarlyBonus);
	addHeading3(mGuiSystem, new DualTextItem{"Early bonus: ", tmp, alignLine, hAlign});

	std::snprintf(tmp, sizeof(tmp), "%i  (x%i pts)", results.objectsShift, lastModelCfg.objectShiftBonus);
	addHeading3(mGuiSystem, new DualTextItem{"Shift bonus: ", tmp, alignLine, hAlign});


	addHeading2(mGuiSystem, new TextItem{"Bonus objects"});

	std::snprintf(tmp, sizeof(tmp), "%i  (x%i pts)", results.bonusObjectsEaten, lastModelCfg.bonusObjectValue);
	addHeading3(mGuiSystem, new DualTextItem{"Eaten: ", tmp, alignLine, hAlign});

	std::snprintf(tmp, sizeof(tmp), "%i  (x%i pts)", results.bonusObjectsShift, lastModelCfg.bonusObjectShiftBonus);
	addHeading3(mGuiSystem, new DualTextItem{"Shift bonus: ", tmp, alignLine, hAlign});

	std::snprintf(tmp, sizeof(tmp), "%i", results.bonusObjectsMissed);
	addHeading3(mGuiSystem, new DualTextItem{"Missed: ", tmp, alignLine, hAlign});
	
	addStandardPadding(mGuiSystem);
	

	addHeading1(mGuiSystem, new TextItem{"Misc stats"});

	std::snprintf(tmp, sizeof(tmp), "%i", results.tilesTraversed);
	addHeading3(mGuiSystem, new DualTextItem{"Tiles traversed: ", tmp, alignLine, hAlign});

	std::snprintf(tmp, sizeof(tmp), "%i", results.numberOfShifts);
	addHeading3(mGuiSystem, new DualTextItem{"Number of shifts: ", tmp, alignLine, hAlign});

	std::snprintf(tmp, sizeof(tmp), "%.2f tiles/second", results.maxSpeed);
	addHeading3(mGuiSystem, new DualTextItem{"Max speed: ", tmp, alignLine, hAlign});

	// Navbar	
	addNavbar(mGuiSystem, new SideSplitContainer{}, restPadding);
	SideSplitContainer& sideSplit = *(SideSplitContainer*)mGuiSystem.items().back().get();
	sideSplit.setLeft(shared_ptr<BaseItem>{new Button{"Retry", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new GameScreen{this->lastModelConfig}}};
	}}}, MENU_DIM.x * 0.4f);
	sideSplit.setRight(shared_ptr<BaseItem>{new Button{"Main Menu", [this](Button&) {
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new MainMenuScreen{}}};
	}}}, MENU_DIM.x * 0.4f);
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