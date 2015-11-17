#include "rendering/Spotlight.hpp"

#include <sfz/math/MathConstants.hpp>

namespace s3 {

mat4 Spotlight::viewMatrix() const noexcept
{
	vec3 up = cross(pos, dir);
	if (up == vec3(0.0)) up = vec3{dir.y, dir.z, dir.x};
	up = normalize(up);
	return sfz::lookAt(pos, pos + dir, up);
}

mat4 Spotlight::projMatrix() const noexcept
{
	return sfz::glPerspectiveProjectionMatrix(fovDeg, 1.0, near, range);
}

mat4 Spotlight::lightMatrix(const mat4& inverseViewMatrix) const noexcept
{
	static const mat4 translScale = sfz::translationMatrix(0.5f, 0.5f, 0.5f)
	                              * sfz::scalingMatrix4(0.5f);
	return translScale * this->projMatrix() * this->viewMatrix() * inverseViewMatrix;
}


void stupidSetSpotLightUniform(const gl::Program& program, const char* name, const Spotlight& spotlight,
                               const mat4& viewMatrix, const mat4& invViewMatrix) noexcept
{
	using std::snprintf;
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "vsPos");
	gl::setUniform(program, buffer, transformPoint(viewMatrix, spotlight.pos));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "vsDir");
	gl::setUniform(program, buffer, transformDir(viewMatrix, spotlight.dir));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "color");
	gl::setUniform(program, buffer, spotlight.color);
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "range");
	gl::setUniform(program, buffer, spotlight.range);
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "fovRad");
	gl::setUniform(program, buffer, spotlight.fovDeg * sfz::DEG_TO_RAD());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "softAngleRad");
	gl::setUniform(program, buffer, spotlight.softAngleDeg * sfz::DEG_TO_RAD());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "lightMatrix");
	gl::setUniform(program, buffer, spotlight.lightMatrix(invViewMatrix));
}

} // namespace s3