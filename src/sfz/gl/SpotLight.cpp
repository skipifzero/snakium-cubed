#include "sfz/gl/Spotlight.hpp"

#include <sfz/math/MathConstants.hpp>

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static vec3 genUpVector(vec3 dir) noexcept
{
	vec3 up{0.0f, 1.0f, 0.0f};
	vec3 dirNorm = normalize(dir);
	float cos = dot(up, dirNorm);
	if (cos < 0.025f || 0.975f < cos) {
		up = vec3{1.0f, 0.0f, 0.0f};
	}
	return up;
}

// Spotlight: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Spotlight::Spotlight(vec3 pos, vec3 dir, float softFovDeg, float sharpFovDeg, float range,
                     float near, vec3 color) noexcept
{
	sfz_assert_debug(dir != vec3{0.0f});
	sfz_assert_debug(0.0f < sharpFovDeg);
	sfz_assert_debug(sharpFovDeg <= softFovDeg);
	sfz_assert_debug(sharpFovDeg < 180.0f);
	sfz_assert_debug(0.0f < near);
	sfz_assert_debug(near < range);

	vec3 up = genUpVector(dir);
	mViewFrustum = ViewFrustum{pos, dir, up, softFovDeg, 1.0f, near, range};
	mViewFrustumMesh = ViewFrustumMesh{mViewFrustum};
	mSharpFovDeg = sharpFovDeg;
	mColor = color;
}

// Spotlight: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

mat4 Spotlight::lightMatrix(const mat4& inverseViewMatrix) const noexcept
{
	static const mat4 translScale = sfz::translationMatrix(0.5f, 0.5f, 0.5f)
	                              * sfz::scalingMatrix4(0.5f);
	return translScale * mViewFrustum.projMatrix() * mViewFrustum.viewMatrix() * inverseViewMatrix;
}

void Spotlight::renderViewFrustum() noexcept
{
	mViewFrustumMesh.render();
}

mat4 Spotlight::viewFrustumTransform() const noexcept
{
	return mViewFrustumMesh.generateTransform(mViewFrustum);
}

// Spotlight: Setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Spotlight::pos(vec3 pos) noexcept
{
	mViewFrustum.setPos(pos);
}

void Spotlight::dir(vec3 dir) noexcept
{
	sfz_assert_debug(dir != vec3{0.0f});
	vec3 up = genUpVector(dir);
	mViewFrustum.setDir(dir, up);
}

void Spotlight::softFov(float softFovDeg) noexcept
{
	sfz_assert_debug(mSharpFovDeg <= softFovDeg);
	mViewFrustum.setVerticalFov(softFovDeg);
	mViewFrustumMesh = ViewFrustumMesh{mViewFrustum};
}

void Spotlight::sharpFov(float sharpFovDeg) noexcept
{
	sfz_assert_debug(0.0f < sharpFovDeg);
	sfz_assert_debug(sharpFovDeg < mViewFrustum.verticalFov());
	mSharpFovDeg = sharpFovDeg;
}

void Spotlight::range(float range) noexcept
{
	sfz_assert_debug(mViewFrustum.near() < range);
	mViewFrustum.setClipDist(mViewFrustum.near(), range);
	mViewFrustumMesh = ViewFrustumMesh{mViewFrustum};
}

void Spotlight::near(float near) noexcept
{
	sfz_assert_debug(near < mViewFrustum.far());
	mViewFrustum.setClipDist(near, mViewFrustum.far());
	mViewFrustumMesh = ViewFrustumMesh{mViewFrustum};
}

void Spotlight::color(vec3 color) noexcept
{
	mColor = color;
}

} // namespace