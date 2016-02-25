#include "screens/NewHighScoreScreen.hpp"

#include <random> 

#include <sfz/geometry/AABB2D.hpp>
#include <sfz/GL.hpp>
#include <sfz/gl/OpenGL.hpp>

#include "screens/MenuConstants.hpp"
#include "screens/ResultScreen.hpp"
#include "screens/ScreenUtils.hpp"

namespace s3 {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static std::mt19937_64 createRNGGenerator(void) noexcept
{
	static std::random_device rnd_dev;
	return std::mt19937_64(rnd_dev());
}

static bool isValidScoreConfig(const ModelConfig& cfg) noexcept
{
	return cfg == STANDARD_CONFIG || cfg == LARGE_CONFIG || cfg == GIANT_CONFIG;
}

static ScoreConfigType cfgToType(const ModelConfig& cfg) noexcept
{
	if (cfg == STANDARD_CONFIG) return ScoreConfigType::STANDARD;
	if (cfg == LARGE_CONFIG) return ScoreConfigType::LARGE;
	if (cfg == GIANT_CONFIG) return ScoreConfigType::GIANT;
	sfz_error("Logic error! Impossible config!");
}

// NewHighScoreScreen: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

NewHighScoreScreen::NewHighScoreScreen(const ModelConfig& lastModelCfg, const Stats& results) noexcept
:
	lastModelConfig{lastModelCfg},
	results{results},
	mGuiSystem{sfz::Rectangle{MENU_SYSTEM_DIM/2.0f, MENU_SYSTEM_DIM}}
{
	using namespace gui;
	snprintf(mNameStr, sizeof(mNameStr), "");
	mNameStrIndex = 0;
	
	if (!isValidScoreConfig(lastModelCfg)) {
		mIsHighScore = false;
		return;
	}

	ScoreBundle scores = createEmptyScoreBundle();
	loadScores(scores);
	int rank = checkScoreRank(scores, cfgToType(lastModelCfg), results);
	if (rank == -1) {
		mIsHighScore = false;
		return;
	}
	mIsHighScore = true;

	char tmp[256];
	const float strWidth = MENU_DIM.x * 0.8f;

	// Title
	addTitle(mGuiSystem, new TextItem{"High Score"});

	// Content
	std::snprintf(tmp, sizeof(tmp), "Score: %i", totalScore(results, lastModelCfg));
	addHeading1(mGuiSystem, new TextItem{tmp, HorizontalAlign::LEFT}, strWidth, HorizontalAlign::CENTER);

	const char* rankStr = rank == 1 ? "Ranked 1st" : rank == 2 ? "Ranked 2nd" : "Ranked 3rd";
	addHeading3(mGuiSystem, new TextItem{rankStr, HorizontalAlign::LEFT}, strWidth, HorizontalAlign::CENTER);

	addStandardPadding(mGuiSystem);
	mNameItemPtr = new TextItem{"Enter name: ", HorizontalAlign::LEFT};
	addHeading2(mGuiSystem, mNameItemPtr, strWidth, HorizontalAlign::CENTER);

	addStandardPadding(mGuiSystem);
	addHeading2(mGuiSystem, new Button{"Randomize name", [this](Button&) {
		std::mt19937_64 ms = createRNGGenerator();
		std::uniform_int_distribution<size_t> lenDist{3, SCORE_NAME_LENGTH};
		
		std::uniform_int_distribution<char> charDist{97, 122};
		size_t nameLength = lenDist(ms);
		for (size_t i = 0; i < nameLength; ++i) {
			this->mNameStr[i] = charDist(ms);
		}
		this->mNameStr[nameLength] = '\0'; 
		this->mNameStrIndex = (uint32_t)nameLength;
		this->updateNameItem();
	}}, MENU_DIM.x * 0.4f);

	// Navbar
	const float restPadding = calcRestPadding(1.0f, 2.0f, 1.0f, 2.0f);
	addNavbar(mGuiSystem, new Button{"Save", [this, scores](Button&) {
		auto tmpScores = scores;
		int rank = tryAddScoreToBundle(tmpScores, cfgToType(this->lastModelConfig), this->results, this->mNameStr);
		if (rank != -1) {
			if (writeScores(tmpScores)) {
				std::cout << "Wrote scores to file!\n";
			}
		}
		this->mUpdateOp = UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN,
		                           shared_ptr<BaseScreen>{new ResultScreen{this->lastModelConfig, this->results, false}}};
	}}, restPadding, MENU_DIM.x * 0.4f);
	mGuiSystem.items().back()->disable();
}

// NewHighScoreScreen: Overriden screen methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

UpdateOp NewHighScoreScreen::update(UpdateState& state)
{
	if (!mIsHighScore) {
		return UpdateOp{sfz::UpdateOpType::SWITCH_SCREEN, shared_ptr<BaseScreen>{new ResultScreen{lastModelConfig, results, false}}};
	}

	const vec2 drawableDim = state.window.drawableDimensions();
	const sfz::AABB2D guiCam = gui::calculateGUICamera(drawableDim, MENU_SYSTEM_DIM);

	// Remove all events temporarily so keyboard can't be used
	auto eventsBackup = std::move(state.events);

	// Re-add a few events
	for (auto& event : eventsBackup) {
		if (event.type == SDL_KEYUP) {
			int key = event.key.keysym.sym;
			if (key == SDLK_DOWN || key == SDLK_UP || key == SDLK_LEFT || key == SDLK_RIGHT || key == SDLK_RETURN) {
				state.events.push_back(event);
			}
		}
	}

	int32_t ctrlId = getFirstController(state);
	gui::InputData data = inputDataFromUpdateState(state, guiCam, ctrlId, nullptr);
	mGuiSystem.update(data, state.delta);
	
	state.events = std::move(eventsBackup);

	// Check if shift
	for (auto& event : state.events) {
		if (event.type == SDL_KEYDOWN) {
			int key = event.key.keysym.sym;
			if (key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
				mShift = true;
				break;
			}
		}
		if (event.type == SDL_KEYUP) {
			int key = event.key.keysym.sym;
			if (key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
				mShift = false;
				break;
			}
		}
	}

	// Character entry
	for (auto& event : state.events) {
		if (event.type != SDL_KEYDOWN) continue;
		int key = event.key.keysym.sym;

		if (key == SDLK_BACKSPACE && mNameStrIndex > 0) {
			mNameStr[mNameStrIndex-1] = '\0';
			mNameStrIndex -= 1;
			updateNameItem();
		}

		// Handle letters a-z
		if ((97 <= key && key <= 122) && mNameStrIndex < SCORE_NAME_LENGTH) {
			mNameStr[mNameStrIndex] = mShift ? key - 32 : key;
			mNameStrIndex += 1;
			mNameStr[mNameStrIndex] = '\0';
			updateNameItem();
		}

		// Other characters
		else if ((key == 32 || (48 <= key && key <= 57)) && mNameStrIndex < SCORE_NAME_LENGTH) {
			mNameStr[mNameStrIndex] = key;
			mNameStrIndex += 1;
			mNameStr[mNameStrIndex] = '\0';
			updateNameItem();
		}
	}

	return mUpdateOp;
}

void NewHighScoreScreen::render(UpdateState& state)
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

// NewHighScoreScreen: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void NewHighScoreScreen::updateNameItem() noexcept
{
	char tmp[128];
	snprintf(tmp, sizeof(tmp), "Enter name: %s", mNameStr);
	mNameItemPtr->text = tmp;

	if (mNameStrIndex == 0) {
		mGuiSystem.items().back()->disable();
	} else {
		mGuiSystem.items().back()->enable();
	}
}

} // namespace s3