#pragma once
#ifndef S3_SCREENS_RESULT_SCREEN_HPP
#define S3_SCREENS_RESULT_SCREEN_HPP

#include <memory>

#include <sfz/Screens.hpp>
#include <sfz/GUI.hpp>

#include "gamelogic/ModelConfig.hpp"
#include "gamelogic/Stats.hpp"

namespace s3 {

using gui::BaseItem;
using sfz::UpdateOp;
using sfz::UpdateState;
using std::shared_ptr;

class ResultScreen : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ResultScreen() noexcept = delete;
	ResultScreen(const ResultScreen&) = delete;
	ResultScreen& operator= (const ResultScreen&) = delete;
	~ResultScreen() noexcept = default;

	ResultScreen(const ModelConfig& lastModelCfg, const Stats& results) noexcept;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(UpdateState& state) override final;
	virtual void render(UpdateState& state) override final;

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	const ModelConfig lastModelConfig;
	const Stats results;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gui::System mGuiSystem;
	UpdateOp mUpdateOp = sfz::SCREEN_NO_OP;

	shared_ptr<BaseItem> mRetryExitCon, mRetryButton, mExitButton;
};

} // namespace s3
#endif