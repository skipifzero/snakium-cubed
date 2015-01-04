#pragma once
#ifndef S3_CAMERA_HPP
#define S3_CAMERA_HPP

#include "sfz/Math.hpp"
#include "GameLogic.hpp"

namespace s3 {

class Camera {
public:
	sfz::mat4f mViewMatrix;
	Direction3D mUpDir = s3::Direction3D::UP;
	float mFov = 60.0f;

	void update(const Model& model) noexcept;

private:
	Direction3D lastCubeSide = s3::Direction3D::SOUTH;
	sfz::vec3f mPos, mUp;
};

} // namespace s3

#endif