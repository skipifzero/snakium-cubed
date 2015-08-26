#pragma once
#ifndef S3_SCREENS_MAIN_MENU_SCREEN_HPP
#define S3_SCREENS_MAIN_MENU_SCREEN_HPP

#include <memory>

#include <sfz/GL.hpp>
#include <sfz/Screens.hpp>
#include <sfz/GUI.hpp>

#include "Assets.hpp"
#include "GlobalConfig.hpp"
#include "screens/GameScreen.hpp"
#include "screens/ScreenMenuConstants.hpp"

namespace s3 {

using sfz::UpdateOp;
using sfz::UpdateState;
using sfz::vec2;
using sfz::vec3;

using std::shared_ptr;
using std::vector;

class MainMenuScreen : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	MainMenuScreen() noexcept = delete;
	MainMenuScreen& operator= (const MainMenuScreen&) noexcept = delete;

	MainMenuScreen(sdl::Window& window, Assets& assets) noexcept;
	~MainMenuScreen() noexcept = default;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(const UpdateState& state) override final;
	virtual void render(const UpdateState& state) override final;
	virtual void onQuit() override final;
	virtual void onResize(vec2 dimensions, vec2 drawableDimensions) override final;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GlobalConfig& mCfg = GlobalConfig::INSTANCE();
	sdl::Window& mWindow;
	Assets& mAssets;
	bool mQuit = false;

	sfz::Button mNewGameButton, mQuitButton;
};

} // namespace s3

#endif