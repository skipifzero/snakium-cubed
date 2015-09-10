#pragma once
#ifndef S3_SCREENS_OPTIONS_SCREEN_HPP
#define S3_SCREENS_OPTIONS_SCREEN_HPP

#include <sfz/Screens.hpp>
#include <sfz/GUI.hpp>

namespace s3 {

using sfz::UpdateOp;
using sfz::UpdateState;
using sfz::vec2;
using sfz::vec3;

class OptionsScreen : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	OptionsScreen(const OptionsScreen&) = delete;
	OptionsScreen& operator= (const OptionsScreen&) = delete;
	~OptionsScreen() noexcept = default;

	OptionsScreen() noexcept;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(const UpdateState& state) override final;
	virtual void render(const UpdateState& state) override final;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool testBool; // TODO: REMOVE

	gui::System mGuiSystem;
	UpdateOp mUpdateOp = sfz::SCREEN_NO_OP;
};

} // namespace s3

#endif