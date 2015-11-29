#include "sfz/geometry/ViewFrustum.hpp"

#include <sfz/geometry/AABB.hpp>
#include <sfz/geometry/Intersection.hpp>
#include <sfz/geometry/OBB.hpp>
#include <sfz/geometry/Sphere.hpp>

namespace sfz {

// Statiics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static OBB obbApproximation(const ViewFrustum& frustum) noexcept
{
	using std::tanf;
	const float yHalfRadAngle = (frustum.verticalFov() / 2.0f) * sfz::DEG_TO_RAD();
	const float xHalfRadAngle = frustum.aspectRatio() * yHalfRadAngle;
	const float nearMFar = frustum.far() - frustum.near();
	return OBB{frustum.pos() + frustum.dir() * (frustum.near() + (nearMFar / 2.0f)),
	           cross(frustum.up(), frustum.dir()), frustum.up(), frustum.dir(),
	           frustum.far() * tanf(xHalfRadAngle) * 2.0f, frustum.far() * tanf(yHalfRadAngle) * 2.0f, nearMFar};
}

// ViewFrustum: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ViewFrustum::ViewFrustum(vec3 position, vec3 direction, vec3 up, float verticalFov, float aspect,
                         float near, float far) noexcept
{
	this->set(position, direction, up, verticalFov, aspect, near, far);
}

// ViewFrustum: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool ViewFrustum::isVisible(const AABB& aabb) const noexcept
{
	if (!(belowPlane(mLeftPlane, aabb) && belowPlane(mRightPlane, aabb))) return false;
	if (!(belowPlane(mNearPlane, aabb) && belowPlane(mFarPlane, aabb))) return false;
	if (!(belowPlane(mUpPlane, aabb) && belowPlane(mDownPlane, aabb))) return false;
	return true;
}

bool ViewFrustum::isVisible(const OBB& obb) const noexcept
{
	if (!(belowPlane(mLeftPlane, obb) && belowPlane(mRightPlane, obb))) return false;
	if (!(belowPlane(mNearPlane, obb) && belowPlane(mFarPlane, obb))) return false;
	if (!(belowPlane(mUpPlane, obb) && belowPlane(mDownPlane, obb))) return false;
	return true;
}

bool ViewFrustum::isVisible(const Sphere& sphere) const noexcept
{
	if (!(belowPlane(mLeftPlane, sphere) && belowPlane(mRightPlane, sphere))) return false;
	if (!(belowPlane(mNearPlane, sphere) && belowPlane(mFarPlane, sphere))) return false;
	if (!(belowPlane(mUpPlane, sphere) && belowPlane(mDownPlane, sphere))) return false;
	return true;
}

bool ViewFrustum::isVisible(const ViewFrustum& viewFrustum) const noexcept
{
	// TODO: This sucks. Replace with better algorithm.
	OBB approx = obbApproximation(viewFrustum);
	return this->isVisible(approx);
}

// ViewFrustum: Setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ViewFrustum::setPos(vec3 position) noexcept
{
	mPos = position;
	update();
}

void ViewFrustum::setVerticalFov(float verticalFov) noexcept
{
	sfz_assert_debug(0.0f < verticalFov && verticalFov < 180.0f);
	mVerticalFov = verticalFov;
	update();
}

void ViewFrustum::setAspectRatio(float aspect) noexcept
{
	sfz_assert_debug(0.0f < aspect);
	mAspectRatio = aspect;
	update();
}

void ViewFrustum::setDir(vec3 direction, vec3 up) noexcept
{
	mDir = normalize(direction);
	mUp = normalize(up - dot(up, direction) * direction);
	sfz_assert_debug(approxEqual(dot(mDir, mUp), 0.0f));

	update();
}

void ViewFrustum::setClipDist(float near, float far) noexcept
{
	sfz_assert_debug(0.0f < near);
	mNear = near;
	sfz_assert_debug(near < far);
	mFar = far;
}

void ViewFrustum::set(vec3 position, vec3 direction, vec3 up, float verticalFov, float aspect,
                      float near, float far) noexcept
{
	mPos = position;
	mDir = normalize(direction);
	mUp = normalize(up - dot(up, direction) * direction);
	sfz_assert_debug(approxEqual(dot(mDir, mUp), 0.0f, 0.1f));
	sfz_assert_debug(0.0f < verticalFov && verticalFov < 180.0f);
	mVerticalFov = verticalFov;
	sfz_assert_debug(0.0f < aspect);
	mAspectRatio = aspect;
	sfz_assert_debug(0.0f < near);
	mNear = near;
	sfz_assert_debug(near < far);
	mFar = far;

	update();
}

// ViewFrustum: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ViewFrustum::update() noexcept
{
	updateMatrices();
	updatePlanes();
}

void ViewFrustum::updateMatrices() noexcept
{
	mViewMatrix = lookAt(mPos, mPos + mDir, mUp);
	mProjMatrix = glPerspectiveProjectionMatrix(mVerticalFov, mAspectRatio, mNear, mFar);
}

void ViewFrustum::updatePlanes() noexcept
{
	const vec3 right = normalize(cross(mDir, mUp));
	const float yHalfRadAngle = (mVerticalFov/2.0f) * DEG_TO_RAD();
	const float xHalfRadAngle = mAspectRatio * yHalfRadAngle;
	
	sfz_assert_debug(approxEqual(dot(mDir, mUp), 0.0f));
	sfz_assert_debug(approxEqual(dot(mDir, right), 0.0f));
	sfz_assert_debug(approxEqual(dot(mUp, right), 0.0f));

	mNearPlane = Plane{-mDir, mPos + mDir*mNear};
	mFarPlane = Plane{mDir, mPos + mDir*mFar};
	
	vec3 upPlaneDir = rotationMatrix3(right, yHalfRadAngle) * mDir;
	vec3 upPlaneNormal = normalize(cross(upPlaneDir, -right));
	mUpPlane = Plane{upPlaneNormal, mPos};

	vec3 downPlaneDir = rotationMatrix3(-right, yHalfRadAngle) * mDir;
	vec3 downPlaneNormal = normalize(cross(downPlaneDir, right));
	mDownPlane = Plane{downPlaneNormal, mPos};

	vec3 rightPlaneDir = rotationMatrix3(-mUp, xHalfRadAngle) * mDir;
	vec3 rightPlaneNormal = normalize(cross(rightPlaneDir, mUp));
	mRightPlane = Plane{rightPlaneNormal, mPos};

	vec3 leftPlaneDir = rotationMatrix3(mUp, xHalfRadAngle) * mDir;
	vec3 leftPlaneNormal = normalize(cross(leftPlaneDir, -mUp));
	mLeftPlane = Plane{leftPlaneNormal, mPos};
}

} // namespace sfz