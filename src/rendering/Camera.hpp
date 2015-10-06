#pragma once
#ifndef S3_CAMERA_HPP
#define S3_CAMERA_HPP

#include <sfz/math/Matrix.hpp>
#include <sfz/math/Vector.hpp>

#include "gamelogic/Direction.hpp"
#include "gamelogic/Model.hpp"

namespace s3 {

using sfz::mat4;
using sfz::vec3;

class Camera {
public:
	mat4 mViewMatrix;
	Direction mUpDir = Direction::UP;
	float mFov = 60.0f;
	Direction mSideRenderOrder[6];
	bool mRenderTileFaceFirst[6];

	Camera() noexcept;
	void update(const Model& model, float delta) noexcept;

private:
	Direction mLastCubeSide = Direction::BACKWARD;
	Direction mLastUpDir = Direction::UP;
	vec3 mPos, mUp, mUpTarget;
};

} // namespace s3

#endif