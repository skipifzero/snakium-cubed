#pragma once
#ifndef S3_SCREENS_GAME_SCREEN_HPP
#define S3_SCREENS_GAME_SCREEN_HPP

#include <sfz/GL.hpp>
#include <sfz/Math.hpp>
#include <sfz/Screens.hpp>

#include "Assets.hpp"
#include "Camera.hpp"
#include "GlobalConfig.hpp"
#include "screens/MainMenuScreen.hpp"

namespace s3 {

using sfz::vec2;
using sfz::vec3;
using sfz::vec4;
using sfz::mat3;
using sfz::mat4;

using sfz::ScreenUpdateOp;
using std::unordered_map;
using std::vector;

class GameScreen final : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GameScreen() noexcept = delete;
	GameScreen& operator= (const GameScreen&) noexcept = delete;

	GameScreen(sdl::Window& window, Assets& assets, const ModelConfig& modelCfg) noexcept;
	~GameScreen() noexcept = default;

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

	GLuint mShaderProgram;
	Camera mCam;

	mat4 projMatrix;
	bool isTransparent = false;
	bool isPaused = false;

	GlobalConfig& mCfg = GlobalConfig::INSTANCE();
	sdl::Window& mWindow;
	Assets& mAssets;
	Model mModel;


	bool mQuit = false;
};

} // namespace s3

#endif