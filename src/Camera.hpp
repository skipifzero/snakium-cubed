#pragma once
#ifndef S3_CAMERA_HPP
#define S3_CAMERA_HPP

#include "sfz/Math.hpp"
#include "GameLogic.hpp"
#include <exception> // std::terminate
#include <cmath>

#include "sfz/MSVC12HackON.hpp"

namespace s3 {

class Camera {
public:
	sfz::mat4f mViewMatrix;
	Direction3D mUpDir = Direction3D::UP;
	float mFov = 60.0f;

	Camera() noexcept;
	void update(const Model& model, float delta) noexcept;

private:
	Direction3D mLastCubeSide = Direction3D::SOUTH;
	Direction3D mLastUpDir = Direction3D::UP;
	sfz::vec3f mPos, mUp, mUpTarget;
};

} // namespace s3

#include "sfz/MSVC12HackOFF.hpp"

#endif