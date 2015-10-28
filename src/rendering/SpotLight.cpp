#include "rendering/SpotLight.hpp"

namespace s3 {

mat4 SpotLight::viewMatrix() const noexcept
{
	return sfz::lookAt(pos, pos + dir, normalize(cross(pos, dir)));
}

mat4 SpotLight::projMatrix() const noexcept
{
	return sfz::glPerspectiveProjectionMatrix(angle, 1.0, near, reach);
}

mat4 SpotLight::lightMatrix(const mat4& inverseViewMatrix) const noexcept
{
	static const mat4 translScale = sfz::translationMatrix(0.5f, 0.5f, 0.5f)
	                              * sfz::scalingMatrix4(0.5f);
	return translScale * this->projMatrix() * this->viewMatrix() * inverseViewMatrix;
}

} // namespace s3