#pragma once
#ifndef S3_SCREENS_GAME_SCREEN_HPP
#define S3_SCREENS_GAME_SCREEN_HPP

#include <sfz/GL.hpp>
#include <sfz/Math.hpp>

#include "Assets.hpp"
#include "screens/IScreen.hpp"
#include "Camera.hpp"
#include "GlobalConfig.hpp"

namespace s3 {

using sfz::vec3;
using sfz::vec4;
using sfz::mat3;
using sfz::mat4;

class GameScreen final : public IScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GameScreen() noexcept = delete;
	GameScreen& operator= (const GameScreen&) noexcept = delete;

	GameScreen(sdl::Window& window, Assets& assets, const ModelConfig& modelCfg) noexcept;
	~GameScreen() noexcept = default;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual void update(const std::vector<SDL_Event>& events, float delta) override final;
	virtual IScreen* changeScreen() override final;
	virtual bool quit() override final;
	virtual void render(float delta) override final;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GLuint mShaderProgram;
	Camera mCam;

	mat4 projMatrix;
	bool isTransparent = false;
	bool isPaused = false;

	GlobalConfig mCfg;
	sdl::Window& mWindow;
	Assets& mAssets;
	Model mModel;


	bool mQuit = false;
};

} // namespace s3

#endif