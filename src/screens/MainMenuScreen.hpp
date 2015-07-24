#pragma once
#ifndef S3_SCREENS_MAIN_MENU_SCREEN_HPP
#define S3_SCREENS_MAIN_MENU_SCREEN_HPP

#include <sfz/GL.hpp>

#include "Assets.hpp"
#include "screens/IScreen.hpp"
#include "GlobalConfig.hpp"
#include "screens/GameScreen.hpp"

namespace s3 {

class MainMenuScreen : public IScreen {
public:

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	MainMenuScreen() noexcept = delete;
	MainMenuScreen& operator= (const MainMenuScreen&) noexcept = delete;

	MainMenuScreen(sdl::Window& window, Assets& assets) noexcept;
	~MainMenuScreen() noexcept = default;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual void update(const std::vector<SDL_Event>& events, float delta) override final;
	virtual IScreen* changeScreen() override final;
	virtual bool quit() override final;
	virtual void render(float delta) override final;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GlobalConfig mCfg;
	sdl::Window& mWindow;
	Assets& mAssets;
	bool mQuit = false;
	IScreen* mChangeScreenPtr = nullptr;
};


} // namespace s3

#endif