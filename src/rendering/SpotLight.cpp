#include "rendering/Spotlight.hpp"

#include <sfz/math/MathConstants.hpp>

namespace s3 {

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

void stupidSetSpotLightUniform(const gl::Program& program, const char* name, const Spotlight& spotlight,
                               const mat4& viewMatrix, const mat4& invViewMatrix) noexcept
{
	using std::snprintf;
	char buffer[128];
	const auto& frustum = spotlight.viewFrustum();
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "vsPos");
	gl::setUniform(program, buffer, transformPoint(viewMatrix, frustum.pos()));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "vsDir");
	gl::setUniform(program, buffer, transformDir(viewMatrix, frustum.dir()));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "color");
	gl::setUniform(program, buffer, spotlight.color());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "range");
	gl::setUniform(program, buffer, frustum.far());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "softFovRad");
	gl::setUniform(program, buffer, frustum.verticalFov() * sfz::DEG_TO_RAD());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "sharpFovRad");
	gl::setUniform(program, buffer, spotlight.sharpFov() * sfz::DEG_TO_RAD());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "softAngleCos");
	gl::setUniform(program, buffer, std::cos((frustum.verticalFov() / 2.0f) * sfz::DEG_TO_RAD()));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "sharpAngleCos");
	gl::setUniform(program, buffer, std::cos((spotlight.sharpFov() / 2.0f) * sfz::DEG_TO_RAD()));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "lightMatrix");
	gl::setUniform(program, buffer, spotlight.lightMatrix(invViewMatrix));
}

} // namespace s3