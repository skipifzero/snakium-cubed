#pragma once
#ifndef S3_CAMERA_HPP
#define S3_CAMERA_HPP

#include <sfz/geometry/ViewFrustum.hpp>
#include <sfz/math/Matrix.hpp>
#include <sfz/math/Vector.hpp>

#include "gamelogic/Direction.hpp"
#include "gamelogic/Model.hpp"

namespace s3 {

using sfz::mat4;
using sfz::vec3;
using sfz::ViewFrustum;

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

	inline Direction upDir() const noexcept { return mUpDir; }
	inline float dist() const noexcept { return mCamDist; }
	inline bool delayModelUpdate() const noexcept { return mDiveInProgress; }
	inline const ViewFrustum& viewFrustum() const noexcept { return mViewFrustum; }

	void update(Model& model, float delta) noexcept;
	void onResize(float fov, float aspect) noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	vec3 mCamDir, mCamUp;
	float mCamDist;

	Direction mUpDir, mLastCubeSide;
	vec3 mTargetCamUp;

	bool mDiveInProgress, mDiveFixUpDir, mDiveInvertUpDir;
	vec3 mDiveTargetCamDir, mDiveTargetCamDirRotAxis;

	ViewFrustum mViewFrustum;
};

// Back-to-front sorting
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct RenderOrder final {
	Direction renderOrder[6];
	bool renderTileFaceFirst[6];
};

RenderOrder calculateRenderOrder(vec3 camPos) noexcept;

} // namespace s3

#endif