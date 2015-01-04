#pragma once
#ifndef S3_CAMERA_HPP
#define S3_CAMERA_HPP

#include "sfz/Math.hpp"
#include "GameLogic.hpp"

namespace s3 {

struct Camera {
	sfz::vec3f mPos, mUp;
	float mFov;
	sfz::mat4f mViewMatrix;
	
	void update(Direction3D upDir, const sfz::vec3f& tilePos) noexcept;
};

} // namespace s3

#endif