#include "rendering/Material.hpp"

#include <cstdio>

namespace s3 {

void setUniformMaterial(const gl::Program& program, const char* name, const Material& material) noexcept
{
	char buffer[200];
	std::snprintf(buffer, sizeof(buffer), "%s.%s", name, "ambient");
	gl::setUniform(program, buffer, material.ambient);
	std::snprintf(buffer, sizeof(buffer), "%s.%s", name, "diffuse");
	gl::setUniform(program, buffer, material.diffuse);
	std::snprintf(buffer, sizeof(buffer), "%s.%s", name, "specular");
	gl::setUniform(program, buffer, material.specular);
	std::snprintf(buffer, sizeof(buffer), "%s.%s", name, "emissive");
	gl::setUniform(program, buffer, material.emissive);
	std::snprintf(buffer, sizeof(buffer), "%s.%s", name, "shininess");
	gl::setUniform(program, buffer, material.shininess);
	std::snprintf(buffer, sizeof(buffer), "%s.%s", name, "opaque");
	gl::setUniform(program, buffer, material.opaque);
}


} // namespace s3