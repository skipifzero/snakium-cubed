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

using sfz::ScreenUpdateOp;
using sfz::vec2;
using sfz::vec3;

using std::shared_ptr;
using std::unordered_map;
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

	virtual ScreenUpdateOp update(const vector<SDL_Event>& events,
	                              const unordered_map<int32_t, sdl::GameController>& controllers,
	                              float delta) override final;
	virtual void render(float delta) override final;
	virtual void onQuit() override final;
	virtual void onResize(vec2 dimensions) override final;

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