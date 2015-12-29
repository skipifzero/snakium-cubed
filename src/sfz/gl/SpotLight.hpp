#pragma once
#ifndef SFZ_GL_RENDERING_SPOTLIGHT_HPP
#define SFZ_GL_RENDERING_SPOTLIGHT_HPP

#include <sfz/geometry/ViewFrustum.hpp>
#include <sfz/gl/Program.hpp>
#include <sfz/gl/ViewFrustumMesh.hpp>
#include <sfz/math/Matrix.hpp>
#include <sfz/math/Vector.hpp>

namespace gl {

using sfz::mat4;
using sfz::vec3;
using sfz::ViewFrustum;

/**
 * @brief A class used to represent a spotlight
 *
 * Generates and holds a ViewFrustum and ViewFrustumMesh along the other parameters used to
 * define a spotlight. The up vector will be automatically generated and is not guaranteed to be
 * any specific value, this should not be a problem as long as this is considered properly in
 * shader code.
 *
 * Two angles are used to define a spotlight:
 * softFov: an outer angle defining the absolute edge of the spotlight
 * sharpFov: an inner angle defining the edge to the sharp part of the spotlight
 * In the area between the sharp and soft angle the light decreases in strength, which should
 * be taken care of in shader code. Possibly with smoothstep or a similar function.
 */
class Spotlight final {
public:
	
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Spotlight() noexcept = default;
	Spotlight(const Spotlight&) = delete;
	Spotlight& operator= (const Spotlight&) = delete;
	Spotlight(Spotlight&&) noexcept = default;
	Spotlight& operator= (Spotlight&&) noexcept = default;

	/**
	 * @param pos the position of the spotlight
	 * @param dir the direction of the spotlight
	 * @param softFovDeg the angle (in degrees) defining the edge of the spotlight
	 * @param sharpFovDeg the angle (in degrees) defining the inner edge the spotlight
	 * @param range the range of the spotlight, can also be thought of as the strength
	 * @param near the near plane used for ViewFrustum and ViewFrustumMesh
	 * @param color the color of the light
	 */
	Spotlight(vec3 pos, vec3 dir, float softFovDeg, float sharpFovDeg, float range,
	          float near = 0.01f, vec3 color = vec3{1.0f}) noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	/**
	 * @brief Matrix for transforming (camera) view space coordinate into shadow map coordinate
	 * CameraViewSpace -> WorldSpace -> LightViewSpace -> LightClipSpace -> Scaling&Translating
	 * into [0, 1] range.
	 */
	mat4 lightMatrix(const mat4& inverseViewMatrix) const noexcept;

	/**
	 * @brief Renders a ViewFrustumMesh with the same size as this spotlight
	 * Up vector used is undefined.
	 */
	void renderViewFrustum() noexcept;

	/**
	 * @brief Transform (model matrix) for this spotlight to be used when rendering the
	 * ViewFrustumMesh with the renderViewFrustum() method. 
	 */
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

} // namespace gl
#endif