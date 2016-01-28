#pragma once
#ifndef S3_SCREENS_MODE_SELECT_SCREEN_HPP
#define S3_SCREENS_MODE_SELECT_SCREEN_HPP

#include <sfz/Screens.hpp>
#include <sfz/GUI.hpp>

namespace s3 {

using sfz::UpdateOp;
using sfz::UpdateState;

class ModeSelectScreen : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ModeSelectScreen(const ModeSelectScreen&) = delete;
	ModeSelectScreen& operator= (const ModeSelectScreen&) = delete;
	~ModeSelectScreen() noexcept = default;


	ModeSelectScreen() noexcept;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(UpdateState& state) override final;
	virtual void render(UpdateState& state) override final;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gui::System mGuiSystem;
	UpdateOp mUpdateOp = sfz::SCREEN_NO_OP;
};

} // namespace s3
#endif