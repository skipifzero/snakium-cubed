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

	GameScreen(sdl::Window& window, s3::Assets& assets, GlobalConfig& cfg) noexcept;
	~GameScreen() noexcept = default;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual void update(const std::vector<SDL_Event>& events, float delta) override final;
	virtual IScreen* changeScreen() override final;
	virtual bool quit() override final;
	virtual void render(float delta) override final;

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool handleInput(s3::Model& model, const SDL_Event& event);
	bool update(s3::Model& model, float delta);
	void render(sdl::Window& window, s3::Assets& assets, s3::Model& model, float);


	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GLuint shaderProgram;

	Camera cam;
	mat4 projMatrix;
	bool isTransparent = false;
	bool isPaused = false;

	sdl::Window& mWindow;
	s3::Assets& mAssets;
	GlobalConfig mCfg;
	s3::Model mModel;


	bool mQuit = false;
};

} // namespace s3

#endif