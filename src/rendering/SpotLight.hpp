#pragma once
#ifndef S3_RENDERING_SPOTLIGHT_HPP
#define S3_RENDERING_SPOTLIGHT_HPP

#include <sfz/geometry/ViewFrustum.hpp>
#include <sfz/gl/Program.hpp>
#include <sfz/gl/ViewFrustumMesh.hpp>
#include <sfz/math/Matrix.hpp>
#include <sfz/math/Vector.hpp>

namespace s3 {

using gl::ViewFrustumMesh;
using sfz::mat4;
using sfz::vec3;
using sfz::ViewFrustum;

class Spotlight final {
public:
	
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Spotlight() noexcept = default;
	Spotlight(const Spotlight&) = delete;
	Spotlight& operator= (const Spotlight&) = delete;
	Spotlight(Spotlight&&) noexcept = default;
	Spotlight& operator= (Spotlight&&) noexcept = default;

	Spotlight(vec3 pos, vec3 dir, float softFovDeg, float sharpFovDeg, float range,
	          float near = 0.01f, vec3 color = vec3{1.0f}) noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	mat4 lightMatrix(const mat4& inverseViewMatrix) const noexcept;
	void renderViewFrustum() noexcept;
	mat4 viewFrustumTransform() const noexcept;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline const ViewFrustum& viewFrustum() const noexcept { return mViewFrustum; }	
	inline float sharpFov() const noexcept { return mSharpFovDeg; }
	inline vec3 color() const noexcept { return mColor; }

	// Setters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
	void pos(vec3 pos) noexcept;
	void dir(vec3 dir) noexcept;
	void softFov(float softFovDeg) noexcept;
	void sharpFov(float sharpFovDeg) noexcept;
	void range(float range) noexcept;
	void near(float near) noexcept;
	void color(vec3 color) noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ViewFrustum mViewFrustum;
	ViewFrustumMesh mViewFrustumMesh;
	float mSharpFovDeg; // The inner fov with sharp light, 0 <= mSharpFov <= mViewFrustum.fov()
	vec3 mColor;
};

void stupidSetSpotLightUniform(const gl::Program& program, const char* name, const Spotlight& spotlight,
                               const mat4& viewMatrix, const mat4& invViewMatrix) noexcept;

} // namespace s3
#endif