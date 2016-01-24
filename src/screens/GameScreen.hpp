#pragma once
#ifndef S3_SCREENS_GAME_SCREEN_HPP
#define S3_SCREENS_GAME_SCREEN_HPP

#include <sfz/gl/Program.hpp>
#include <sfz/math/Matrix.hpp>
#include <sfz/math/Vector.hpp>
#include <sfz/Screens.hpp>
#include <sfz/util/FrametimeStats.hpp>

#include "rendering/Camera.hpp"
#include "rendering/ClassicRenderer.hpp"
#include "rendering/ModernRenderer.hpp"
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

	virtual UpdateOp update(UpdateState& state) override final;
	virtual void render(UpdateState& state) override final;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Model mModel;
	Camera mCam;
	ClassicRenderer mClassicRenderer;
	ModernRenderer mModernRenderer;
	bool mUseModernRenderer = true;
	bool mIsPaused = false;
	float mTimeSinceGameOver = 0.0f;

	DirectionInput mInputBuffer[5];
	size_t mInputBufferIndex = 0;

	sfz::FrametimeStats mShortTermPerfStats, mLongerTermPerfStats, mLongestTermPerfStats;
};

} // namespace s3
#endif