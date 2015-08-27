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

	MainMenuScreen() noexcept = delete;
	MainMenuScreen& operator= (const MainMenuScreen&) noexcept = delete;

	MainMenuScreen(sdl::Window& window) noexcept;
	~MainMenuScreen() noexcept = default;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(const UpdateState& state) override final;
	virtual void render(const UpdateState& state) override final;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Window& mWindow;
	sfz::Button mNewGameButton, mQuitButton;
};

} // namespace s3

#endif