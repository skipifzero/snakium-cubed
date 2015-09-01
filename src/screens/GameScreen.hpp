#pragma once
#ifndef S3_SCREENS_GAME_SCREEN_HPP
#define S3_SCREENS_GAME_SCREEN_HPP

#include <sfz/Math.hpp>
#include <sfz/Screens.hpp>

#include "Camera.hpp"
#include "rendering/TileObject.hpp"

namespace s3 {

using sfz::UpdateOp;
using sfz::UpdateState;

using sfz::vec2;
using sfz::vec3;
using sfz::vec4;
using sfz::mat3;
using sfz::mat4;

class GameScreen final : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GameScreen() noexcept = delete;
	GameScreen& operator= (const GameScreen&) noexcept = delete;

	GameScreen(const ModelConfig& modelCfg) noexcept;
	~GameScreen() noexcept = default;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(const UpdateState& state) override final;
	virtual void render(const UpdateState& state) override final;
	virtual void onResize(vec2 dimensions, vec2 drawableDimensions) override final;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Model mModel;
	TileObject mTile, mXFlippedTile;

	unsigned int mShaderProgram;
	Camera mCam;

	mat4 mProjMatrix;
	bool mIsTransparent = false;
	bool mIsPaused = false;
};

} // namespace s3

#endif