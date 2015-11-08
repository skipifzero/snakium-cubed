#include "rendering/SpotLight.hpp"

namespace s3 {

mat4 SpotLight::viewMatrix() const noexcept
{
	vec3 up = cross(pos, dir);
	if (up == vec3(0.0)) up = vec3{dir.y, dir.z, dir.x};
	up = normalize(up);
	return sfz::lookAt(pos, pos + dir, up);
}

mat4 SpotLight::projMatrix() const noexcept
{
	return sfz::glPerspectiveProjectionMatrix(fov, 1.0, near, range);
}

mat4 SpotLight::lightMatrix(const mat4& inverseViewMatrix) const noexcept
{
	static const mat4 translScale = sfz::translationMatrix(0.5f, 0.5f, 0.5f)
	                              * sfz::scalingMatrix4(0.5f);
	return translScale * this->projMatrix() * this->viewMatrix() * inverseViewMatrix;
}

} // namespace s3