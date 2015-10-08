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

// Camera class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Camera {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Camera(const Camera&) noexcept = default;
	Camera& operator= (const Camera&) noexcept = default;

	Camera() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline mat4 viewMatrix() const noexcept { return mViewMatrix; }
	inline mat4 projMatrix() const noexcept { return mProjMatrix; }
	Direction sideRenderOrder[6];
	bool renderTileFaceFirst[6];
	inline Direction upDir() const noexcept { return mUpDir; }
	inline bool delayModelUpdate() const noexcept { return mDelayModelUpdate; }

	void update(const Model& model, float delta) noexcept;


private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *



	Direction mLastCubeSide = Direction::BACKWARD;
	Direction mLastUpDir = Direction::UP;
	vec3 mUpTarget;

	

	vec3 mPos, mTarget{0.0f}, mUp;
	float mFov = 60.0f, mAspect = 1.0f, mNear = 0.25f, mFar = 5.0f;
	mat4 mViewMatrix, mProjMatrix;
	Direction mUpDir = Direction::UP;
	bool mDelayModelUpdate = false;

};

} // namespace s3

#endif