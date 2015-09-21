#pragma once
#ifndef S3_SCREENS_MAIN_MENU_SCREEN_HPP
#define S3_SCREENS_MAIN_MENU_SCREEN_HPP

#include <sfz/Screens.hpp>
#include <sfz/GUI.hpp>

namespace s3 {

using sfz::UpdateOp;
using sfz::UpdateState;
using sfz::vec2;
using sfz::vec3;

class MainMenuScreen : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	MainMenuScreen(const MainMenuScreen&) = delete;
	MainMenuScreen& operator= (const MainMenuScreen&) = delete;
	~MainMenuScreen() noexcept = default;

	MainMenuScreen() noexcept;

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