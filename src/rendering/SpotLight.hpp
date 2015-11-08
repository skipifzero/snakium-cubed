#pragma once
#ifndef S3_RENDERING_SPOT_LIGHT_HPP
#define S3_RENDERING_SPOT_LIGHT_HPP

#include <sfz/math/Matrix.hpp>
#include <sfz/math/Vector.hpp>

namespace s3 {

using sfz::mat4;
using sfz::vec3;

struct SpotLight final {
	vec3 pos, dir;
	vec3 color;
	float range, fov;

	float near = 0.01;

	mat4 viewMatrix() const noexcept;
	mat4 projMatrix() const noexcept;
	mat4 lightMatrix(const mat4& inverseViewMatrix) const noexcept;
};



} // namespace s3
#endif