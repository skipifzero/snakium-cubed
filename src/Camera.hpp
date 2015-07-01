#pragma once
#ifndef S3_CAMERA_HPP
#define S3_CAMERA_HPP

#include "sfz/Math.hpp"
#include "GameLogic.hpp"
#include <exception> // std::terminate
#include <cmath>

namespace s3 {

class Camera {
public:
	sfz::mat4 mViewMatrix;
	Direction3D mUpDir = Direction3D::UP;
	float mFov = 60.0f;
	Direction3D mSideRenderOrder[6];
	bool mRenderTileFaceFirst[6];

	Camera() noexcept;
	void update(const Model& model, float delta) noexcept;

private:
	Direction3D mLastCubeSide = Direction3D::SOUTH;
	Direction3D mLastUpDir = Direction3D::UP;
	sfz::vec3 mPos, mUp, mUpTarget;
};

} // namespace s3

#endif