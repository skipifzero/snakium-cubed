#pragma once
#ifndef SFZ_GEOMETRY_VIEW_FRUSTUM_HPP
#define SFZ_GEOMETRY_VIEW_FRUSTUM_HPP

#include <sfz/geometry/Plane.hpp>
#include <sfz/geometry/ViewFrustum.hpp>
#include <sfz/math/Vector.hpp>

namespace sfz {
	
// Forward declares geometry primitives
class AABB;
class OBB;
class Sphere;

class ViewFrustum final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ViewFrustum() noexcept = default;
	ViewFrustum(const ViewFrustum&) noexcept = default;
	ViewFrustum& operator= (const ViewFrustum&) noexcept = default;

	ViewFrustum(vec3 position, vec3 direction, vec3 up, float verticalFov, float aspect,
	            float near, float far) noexcept;
	
	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool isVisible(const AABB& aabb) const noexcept;
	bool isVisible(const OBB& obb) const noexcept;
	bool isVisible(const Sphere& sphere) const noexcept;
	bool isVisible(const ViewFrustum& viewFrustum) const noexcept;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline vec3 pos() const noexcept { return mPos; }
	inline vec3 dir() const noexcept { return mDir; }
	inline vec3 up() const noexcept { return mUp; }
	inline float verticalFov() const noexcept { return mVerticalFov; }
	inline float aspectRatio() const noexcept { return mAspectRatio; }
	inline float near() const noexcept { return mNear; }
	inline float far() const noexcept { return mFar; }
	inline const mat4& viewMatrix() const noexcept { return mViewMatrix; }
	inline const mat4& projMatrix() const noexcept { return mProjMatrix; }

	// Setters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void setPos(vec3 position) noexcept;
	void setVerticalFov(float verticalFov) noexcept;
	void setAspectRatio(float aspect) noexcept;

	void setDir(vec3 direction, vec3 up) noexcept;
	void setClipDist(float near, float far) noexcept;
	void set(vec3 position, vec3 direction, vec3 up, float verticalFov, float aspect, float near,
	         float far) noexcept;

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void update() noexcept;
	void updateMatrices() noexcept;
	void updatePlanes() noexcept;

	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	vec3 mPos, mDir, mUp;
	float mVerticalFov, mAspectRatio, mNear, mFar;
	mat4 mViewMatrix, mProjMatrix;
	Plane mNearPlane, mFarPlane, mUpPlane, mDownPlane, mLeftPlane, mRightPlane;
};

} // namespace sfz
#endif