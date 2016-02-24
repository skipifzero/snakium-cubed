#pragma once
#ifndef S3_SCREENS_NEW_HIGH_SCORE_SCREEN_HPP
#define S3_SCREENS_NEW_HIGH_SCORE_SCREEN_HPP

#include <memory>

#include <sfz/Screens.hpp>
#include <sfz/GUI.hpp>

#include "gamelogic/ModelConfig.hpp"
#include "gamelogic/Stats.hpp"

#include "ScoreManagement.hpp"

namespace s3 {

using gui::BaseItem;
using sfz::UpdateOp;
using sfz::UpdateState;
using std::shared_ptr;

class NewHighScoreScreen : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	NewHighScoreScreen() noexcept = delete;
	NewHighScoreScreen(const NewHighScoreScreen&) = delete;
	NewHighScoreScreen& operator= (const NewHighScoreScreen&) = delete;
	~NewHighScoreScreen() noexcept = default;

	NewHighScoreScreen(const ModelConfig& lastModelCfg, const Stats& results) noexcept;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(UpdateState& state) override final;
	virtual void render(UpdateState& state) override final;

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	const ModelConfig lastModelConfig;
	const Stats results;

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void updateNameItem() noexcept;

	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gui::System mGuiSystem;
	UpdateOp mUpdateOp = sfz::SCREEN_NO_OP;
	bool mIsHighScore;
	char mNameStr[SCORE_NAME_LENGTH+1];
	uint32_t mNameStrIndex;
	gui::TextItem* mNameItemPtr;
	bool mShift = false;
};

} // namespace s3
#endif