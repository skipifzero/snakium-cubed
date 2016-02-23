#pragma once
#ifndef S3_SCREENS_HIGH_SCORE_SCREEN_HPP
#define S3_SCREENS_HIGH_SCORE_SCREEN_HPP

#include <sfz/Screens.hpp>
#include <sfz/GUI.hpp>

#include "ScoreManagement.hpp"

namespace s3 {

using sfz::UpdateOp;
using sfz::UpdateState;

// HighScoreScreen class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class HighScoreScreen final : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	HighScoreScreen(const HighScoreScreen&) = delete;
	HighScoreScreen& operator= (const HighScoreScreen&) = delete;
	~HighScoreScreen() noexcept = default;

	HighScoreScreen() noexcept;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(UpdateState& state) override final;
	virtual void render(UpdateState& state) override final;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gui::System mGuiSystem;
	UpdateOp mUpdateOp = sfz::SCREEN_NO_OP;
	ScoreBundle mScores;
};

} // namespace s3
#endif