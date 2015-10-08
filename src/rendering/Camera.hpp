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
	inline Direction upDir() const noexcept { return mUpDir; }
	inline Direction sideRenderOrder(size_t i) const noexcept { return mSideRenderOrder[i]; }
	inline bool renderTileFaceFirst(size_t i) const noexcept { return mRenderTileFaceFirst[i]; }
	inline bool delayModelUpdate() const noexcept { return mDiveInProgress; }

	void update(Model& model, float delta) noexcept;
	void onResize(float fov, float aspect) noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	vec3 mCamDir, mCamUp;
	float mCamDist;
	float mFov, mAspect, mNear, mFar;
	mat4 mViewMatrix, mProjMatrix;

	Direction mUpDir, mLastCubeSide;
	vec3 mTargetCamUp;

	bool mDiveInProgress, mDiveFixUpDir, mDiveInvertUpDir;
	vec3 mDiveTargetCamDir, mDiveTargetCamDirRotAxis;

	Direction mSideRenderOrder[6];
	bool mRenderTileFaceFirst[6];
};

} // namespace s3

#endif